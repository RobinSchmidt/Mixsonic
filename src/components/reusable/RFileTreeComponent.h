#ifndef RFileTreeComponent_h
#define RFileTreeComponent_h

#include "../../libs/JuceIncludes.h"

/**

A copy/paste'd and modified version version of juce::FileTreeComponent (juce version 2.0).


The copy/paste/modify was necessary (instead of making a subclass) because the
FileListTreeItem class is declared in the .cpp file, so we can't access such FileListTreeItem
objects from a subclass (a subclass only sees TreeViewItem objects, but we need to be able
to retriev the underlying file, so we must be able to do a dynamic cast)

*/

class RFileTreeComponent : public TreeView,
                           public DirectoryContentsDisplayComponent
{
public:
    //==============================================================================
    /** Creates a listbox to show the contents of a specified directory.
    */
    RFileTreeComponent (DirectoryContentsList& listToShow);

    /** Destructor. */
    ~RFileTreeComponent();

    //==============================================================================
    /** Returns the number of files the user has got selected.
        @see getSelectedFile
    */
    int getNumSelectedFiles() const                 { return TreeView::getNumSelectedItems(); }

    /** Returns one of the files that the user has currently selected.
        The index should be in the range 0 to (getNumSelectedFiles() - 1).
        @see getNumSelectedFiles
    */
    File getSelectedFile (int index = 0) const;

    /** Deselects any files that are currently selected. */
    void deselectAllFiles();

    /** Scrolls the list to the top. */
    void scrollToTop();

    /** If the specified file is in the list, it will become the only selected item
        (and if the file isn't in the list, all other items will be deselected). */
    void setSelectedFile (const File&);

    /** Updates the files in the list. */
    void refresh();

    /** Setting a name for this allows tree items to be dragged.

        The string that you pass in here will be returned by the getDragSourceDescription()
        of the items in the tree. For more info, see TreeViewItem::getDragSourceDescription().
    */
    void setDragAndDropDescription (const String& description);

    /** Returns the last value that was set by setDragAndDropDescription().
    */
    const String& getDragAndDropDescription() const noexcept     { return dragAndDropDescription; }



    /** Returns the file that is loacted at a give pixel position or File::nonexistent if 
    none. */
    File getFileAtPixelPosition(int x, int y) const; // added by Robin Schmidt



private:
    //==============================================================================
    String dragAndDropDescription;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RFileTreeComponent);
};


#endif
