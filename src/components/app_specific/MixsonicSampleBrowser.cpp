#include "MixsonicSampleBrowser.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction:

MixsonicSampleBrowser::MixsonicSampleBrowser(const File &sampleDirectoryToUse)
: MixsonicBrowserBase(sampleDirectoryToUse)
{
  playButton->setSymbolIndex(RButton::PLAY);  // move to baseclass

  // customize the inherited headline label:
  //headline->setText("Browser", false);
  headline->setText("Library", false);

  contentsList = new DirectoryContentsList(&fileFilter, browserUpdateThread);
  contentsList->setDirectory(sampleDirectoryToUse, true, true);

  /*
	// a dummy-instruction loop to make sure that the DirectoryContentsList has finished loading when 
	// we draw ourselves (otherwise the tree is sometimes drawn incompletely):
	int dummy = 0;
	while( contentsList->isStillLoading() )
		dummy++;		
    // hangs the app up in windows release builds
  */

  addAndMakeVisible( fileTreeComponent = new RFileTreeComponent(*contentsList) );
  fileTreeComponent->setColour(FileTreeComponent::backgroundColourId, backgroundColor);
  fileTreeComponent->setColour(FileTreeComponent::textColourId,       textColor);
  fileTreeComponent->setColour(FileTreeComponent::linesColourId,      textColor);
  fileTreeComponent->setRootItemVisible(false);
  fileTreeComponent->setDefaultOpenness(false);
  fileTreeComponent->addListener(this); // do we need this?
}

MixsonicSampleBrowser::~MixsonicSampleBrowser()
{
  deleteAllChildren();
  delete contentsList;
}

//-------------------------------------------------------------------------------------------------
// setup:

void MixsonicSampleBrowser::setRootDirectory(const File &newDirectory)
{
  contentsList->setDirectory(newDirectory, true, true);
}

void MixsonicSampleBrowser::addFileBrowserListener(FileBrowserListener *listenerToAdd)
{
  fileTreeComponent->addListener(listenerToAdd);
}

//-------------------------------------------------------------------------------------------------
// inquiry:

const File MixsonicSampleBrowser::getSelectedFile() const
{
  return fileTreeComponent->getSelectedFile();
}

//-------------------------------------------------------------------------------------------------
// callbacks:

void MixsonicSampleBrowser::selectionChanged()
{     
  if( fileFilter.isAudioFile(getSelectedFile()) )
    startPlaybackIfDesired(getSelectedFile());
}

void MixsonicSampleBrowser::fileClicked(const File &file, const MouseEvent &e)
{

}

void MixsonicSampleBrowser::fileDoubleClicked(const File &file)
{

}

void MixsonicSampleBrowser::browserRootChanged(const File &newRoot)
{

}

void MixsonicSampleBrowser::resized()
{
  int x = 0;
  int y = 0;
  int w = getWidth();
  int h = getHeight();

  headline->setBounds(x, 0, w, 28);
  playButton->setBounds(w-32, 4, 20, 20);
  y = headline->getBottom();
  h = getHeight() - y;
  fileTreeComponent->setBounds(x+2, y+2, w-4, h-4);
}

void MixsonicSampleBrowser::paint(Graphics &g)
{
  g.setColour(outlineColor);
  g.drawRect(0.f, 0.f, (float)getWidth(), (float)getHeight(), 2.f);


  /*
  g.fillAll(backgroundColor);
  g.setColour(outlineColor);
  g.drawRect(1.f, 1.f, (float)getWidth()-2.f, (float)getHeight()-2.f, 2.f);
  */

  /*
  g.setColour(backgroundColor);
  g.fillRect(0.f, 28.f, (float)getWidth(), 14.f);
  g.fillRect(0.f, (float)getHeight()-42.f, (float)getWidth(), 14.f);
  */

  /*
  g.setColour(outlineColor);
  g.drawRoundedRectangle(1.5f, 1.5f, (float)getWidth()-3, (float)getHeight()-3, 14.f, 3.f);
  g.drawLine(0.f, 28.f, (float) getWidth(), 28.f, 3.f);
  g.drawLine(0.f, (float)getHeight()-28.f, (float) getWidth(), (float)getHeight()-28.f, 3.f);
  */
}
