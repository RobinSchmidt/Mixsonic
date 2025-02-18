#include "RFileTreeComponent.h"

//Image juce_createIconForFile (const File& file);

//==============================================================================
class RFileListTreeItem   : public TreeViewItem,
                            public TimeSliceClient,
                            public AsyncUpdater,
                            public ChangeListener
{
public:
    RFileListTreeItem (RFileTreeComponent& owner_,
                      DirectoryContentsList* const parentContentsList_,
                      const int indexInContentsList_,
                      const File& file_,
                      TimeSliceThread& thread_)
        : file (file_),
          owner (owner_),
          parentContentsList (parentContentsList_),
          indexInContentsList (indexInContentsList_),
          subContentsList (nullptr, false),
          thread (thread_)
    {
        DirectoryContentsList::FileInfo fileInfo;

        if (parentContentsList_ != nullptr
             && parentContentsList_->getFileInfo (indexInContentsList_, fileInfo))
        {
            fileSize = File::descriptionOfSizeInBytes (fileInfo.fileSize);
            modTime = fileInfo.modificationTime.formatted ("%d %b '%y %H:%M");
            isDirectory = fileInfo.isDirectory;
        }
        else
        {
            isDirectory = true;
        }
    }

    ~RFileListTreeItem()
    {
        thread.removeTimeSliceClient (this);
        clearSubItems();
    }

    //==============================================================================
    bool mightContainSubItems()                 { return isDirectory; }
    String getUniqueName() const                { return file.getFullPathName(); }
    int getItemHeight() const                   { return 22; }

    var getDragSourceDescription()              { return owner.getDragAndDropDescription(); }

    void itemOpennessChanged (bool isNowOpen)
    {
        if (isNowOpen)
        {
            clearSubItems();

            isDirectory = file.isDirectory();

            if (isDirectory)
            {
                if (subContentsList == nullptr)
                {
                    jassert (parentContentsList != nullptr);

                    DirectoryContentsList* const l = new DirectoryContentsList (parentContentsList->getFilter(), thread);
                    l->setDirectory (file, true, true);

                    setSubContentsList (l, true);
                }

                changeListenerCallback (nullptr);
            }
        }
    }

    void setSubContentsList (DirectoryContentsList* newList, const bool canDeleteList)
    {
        OptionalScopedPointer<DirectoryContentsList> newPointer (newList, canDeleteList);
        subContentsList = newPointer;
        newList->addChangeListener (this);
    }

    void changeListenerCallback (ChangeBroadcaster*)
    {
        clearSubItems();

        if (isOpen() && subContentsList != nullptr)
        {
            for (int i = 0; i < subContentsList->getNumFiles(); ++i)
            {
                RFileListTreeItem* const item
                    = new RFileListTreeItem (owner, subContentsList, i, subContentsList->getFile(i), thread);

                addSubItem (item);
            }
        }
    }

    void paintItem (Graphics& g, int width, int height)
    {
      /*
        if (file != File::nonexistent)
        {
            updateIcon (true);

            if (icon.isNull())
                thread.addTimeSliceClient (this);
        }
        */

        /*
        owner.getLookAndFeel().drawFileBrowserRow (g, width, height,
                                                   file.getFileName(),
                                                   &icon, fileSize, modTime,
                                                   isDirectory, isSelected(),
                                                   indexInContentsList, owner);
                                                   */
        owner.getLookAndFeel().drawFileBrowserRow (g, width, height,
                                                   file.getFileName(),
                                                   nullptr, fileSize, modTime,
                                                   isDirectory, isSelected(),
                                                   indexInContentsList, owner);
    }

    void itemClicked (const MouseEvent& e)
    {
        owner.sendMouseClickMessage (file, e);
    }

    void itemDoubleClicked (const MouseEvent& e)
    {
        TreeViewItem::itemDoubleClicked (e);

        owner.sendDoubleClickMessage (file);
    }

    void itemSelectionChanged (bool)
    {
        owner.sendSelectionChangeMessage();
    }

    int useTimeSlice()
    {
        //updateIcon (false);
        return -1;
    }

    void handleAsyncUpdate()
    {
        owner.repaint();
    }

    const File file;

private:
    RFileTreeComponent& owner;
    DirectoryContentsList* parentContentsList;
    int indexInContentsList;
    OptionalScopedPointer<DirectoryContentsList> subContentsList;
    bool isDirectory;
    TimeSliceThread& thread;
    //Image icon;
    String fileSize, modTime;

    
      /*
    void updateIcon (const bool onlyUpdateIfCached)
    {
        if (icon.isNull())
        {
            const int hashCode = (file.getFullPathName() + "_iconCacheSalt").hashCode();
            Image im (ImageCache::getFromHashCode (hashCode));

            if (im.isNull() && ! onlyUpdateIfCached)
            {
                im = juce_createIconForFile (file);

                if (im.isValid())
                    ImageCache::addImageToCache (im, hashCode);
            }

            if (im.isValid())
            {
                icon = im;
                triggerAsyncUpdate();
            }
        }
    }
        */    


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RFileListTreeItem);
};

//==============================================================================
RFileTreeComponent::RFileTreeComponent (DirectoryContentsList& listToShow)
    : DirectoryContentsDisplayComponent (listToShow)
{
    setRootItemVisible (false);
    refresh();
}

RFileTreeComponent::~RFileTreeComponent()
{
    deleteRootItem();
}

void RFileTreeComponent::refresh()
{
    deleteRootItem();

    RFileListTreeItem* const root
        = new RFileListTreeItem (*this, nullptr, 0, fileList.getDirectory(),
                                fileList.getTimeSliceThread());

    root->setSubContentsList (&fileList, false);
    setRootItem (root);
}

//==============================================================================
File RFileTreeComponent::getSelectedFile (const int index) const
{
    const RFileListTreeItem* const item = dynamic_cast <const RFileListTreeItem*> (getSelectedItem (index));

    return item != nullptr ? item->file
                           : File::nonexistent;
}

void RFileTreeComponent::deselectAllFiles()
{
    clearSelectedItems();
}

void RFileTreeComponent::scrollToTop()
{
    getViewport()->getVerticalScrollBar()->setCurrentRangeStart (0);
}

void RFileTreeComponent::setDragAndDropDescription (const String& description)
{
    dragAndDropDescription = description;
}

void RFileTreeComponent::setSelectedFile (const File& target)
{
    for (int i = getNumSelectedItems(); --i >= 0;)
    {
        RFileListTreeItem* t = dynamic_cast <RFileListTreeItem*> (getSelectedItem (i));

        if (t != nullptr && t->file == target)
        {
            t->setSelected (true, true);
            return;
        }
    }

    clearSelectedItems();
}



File  RFileTreeComponent::getFileAtPixelPosition(int x, int y) const
{
  File result = File::nonexistent;

  int itemHeight = getRootItem()->getItemHeight(); // assume the items all have the same height
  int itemY      = -getViewport()->getViewPositionY();

  if( y < itemY )
    return File::nonexistent;

  TreeViewItem*          item = NULL;
  RFileListTreeItem* fileItem = NULL;
  for(int r=0; r<getNumRowsInTree(); r++)
  {
    item     = getItemOnRow(r);
    fileItem = NULL;  // might be multiply assigned in the loop, so re-initialize in each iteration

    if( item != NULL )
    {
      if( y >= itemY && y <= itemY+itemHeight )
      {
        fileItem = dynamic_cast<RFileListTreeItem*> (item);
        if( fileItem != NULL )
        {
          return result = fileItem->file;
        }
      }
    }
    itemY += itemHeight;
  }

  return result;
}
