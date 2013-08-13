#include "MixsonicSubBrowser.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction:

MixsonicSubBrowser::MixsonicSubBrowser(const FileFilter *const fileFilter,                   
                                       TimeSliceThread &threadToUse)                                        
{
  contentsList = new DirectoryContentsList(fileFilter, threadToUse);
  contentsList->setDirectory(getApplicationDirectory(), true, false);
	contentsList->refresh();

  addAndMakeVisible( fileListComponent = new FileListComponent(*contentsList) );
  fileListComponent->setColour(FileListComponent::backgroundColourId, 
                               Skin::getInstance()->backgroundColor);
  fileListComponent->setColour(FileListComponent::outlineColourId, 
                               Skin::getInstance()->outlineColor);
  fileListComponent->addListener(this);
}

MixsonicSubBrowser::~MixsonicSubBrowser()
{
  deleteAllChildren();
  delete contentsList;
}

//-------------------------------------------------------------------------------------------------
// setup:

void MixsonicSubBrowser::setDescriptionField(Label* newDescriptionField)
{

}

void MixsonicSubBrowser::setDirectory(const File &directory, const bool includeDirectories, 
                                               const bool includeFiles)
{
  if( directory != contentsList->getDirectory() )
  {
    contentsList->setDirectory(directory, includeDirectories, includeFiles);
    /*
    while( contentsList->isStillLoading() )
    {
      // wait...
    }
    */
    fileListComponent->selectRow(0);
    fileListComponent->repaint();
  }
}

  
void MixsonicSubBrowser::addFileBrowserListener(FileBrowserListener *listenerToAdd)
{
  fileListComponent->addListener(listenerToAdd);
}

//-------------------------------------------------------------------------------------------------
// inquiry:

const File MixsonicSubBrowser::getSelectedFile() const
{
  return fileListComponent->getSelectedFile();
}

//-------------------------------------------------------------------------------------------------
// callbacks:

void MixsonicSubBrowser::selectionChanged()
{
  sendChangeMessage(); 
}

void MixsonicSubBrowser::fileClicked(const File &file, const MouseEvent &e)
{
  sendChangeMessage();
}

void MixsonicSubBrowser::fileDoubleClicked(const File &file)
{

}

void MixsonicSubBrowser::browserRootChanged(const File &newRoot)
{

}

void MixsonicSubBrowser::resized()
{
  int x = 0;
  int y = 0;
  int w = getWidth();
  int h = getHeight();
  fileListComponent->setBounds(x, y, w, h);
}

void MixsonicSubBrowser::paint(Graphics &g)
{
  //g.setColour(Colours::black);
  //g.drawRect(0, 0, getWidth(), getHeight(), 1);
  //g.fillAll(mixsonicAlmostWhite);
}

