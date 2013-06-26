#include "MixsonicProjectComponent.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction:

MixsonicProjectComponent::MixsonicProjectComponent(SamplePool *samplePoolToUse, 
  const File& rootDirectory)                                                 
: SamplePoolClient(samplePoolToUse), MixsonicSampleBrowser(rootDirectory)
{
  fileTreeComponent->setDefaultOpenness(true);

  // customize the inherited headline label:
  headline->setText("Pool", false);

  // create the additional buttons which are specific to this subclass:
  addAndMakeVisible( createDirectoryButton = new MixsonicButton() );
  createDirectoryButton->setDescription(createDirButtonHelpStr);
  createDirectoryButton->setClickingTogglesState(false);
  createDirectoryButton->addListener(this);

  addAndMakeVisible( eraseSampleButton = new MixsonicButton() );
  eraseSampleButton->setDescription(deleteSampleButtonHelpStr);
  eraseSampleButton->setClickingTogglesState(false);
  eraseSampleButton->addListener(this);

  addAndMakeVisible( newButton = new MixsonicButton() );
  newButton->setDescription(createProjectButtonHelpStr);
  newButton->setClickingTogglesState(false);
  newButton->addListener(this);

  addAndMakeVisible( openButton = new MixsonicButton() );
  openButton->setDescription(openProjectButtonHelpStr);
  openButton->setClickingTogglesState(false);
  openButton->addListener(this);

  addAndMakeVisible( recordButton = new MixsonicButton() );
  recordButton->setDescription(recordButtonHelpStr1);
  recordButton->setClickingTogglesState(false);
  recordButton->addListener(this);

  addAndMakeVisible( saveButton = new MixsonicButton() );
  saveButton->setDescription(saveButtonHelpStr);
  saveButton->setClickingTogglesState(false);
  saveButton->addListener(this);
}

MixsonicProjectComponent::~MixsonicProjectComponent()
{
  deleteAllChildren();
}

//-------------------------------------------------------------------------------------------------
// setup:

void MixsonicProjectComponent::setDescriptionField(Label* newDescriptionField)
{
  createDirectoryButton->setDescriptionField(newDescriptionField);  
  eraseSampleButton->setDescriptionField(newDescriptionField);
  newButton->setDescriptionField(newDescriptionField);
  openButton->setDescriptionField(newDescriptionField);
  pauseButton->setDescriptionField(newDescriptionField);  
  playButton->setDescriptionField(newDescriptionField);  
  recordButton->setDescriptionField(newDescriptionField);  
  rewindButton->setDescriptionField(newDescriptionField);  
  saveButton->setDescriptionField(newDescriptionField);
}

//-------------------------------------------------------------------------------------------------
// inquiry:

const File MixsonicProjectComponent::getCurrentDirectory() const
{
  return contentsList->getDirectory();
}

//-------------------------------------------------------------------------------------------------
// callbacks:

void MixsonicProjectComponent::dropComponent(Component *droppedComponent, int x, int y)
{
  AudioClipComponent* clipComponent = dynamic_cast<AudioClipComponent*>(droppedComponent);
  if( clipComponent != NULL )
  {
    // add the sample to the pool ....here are some locks missing:
    //clipComponent->lockUnderlyingClip();
    clipComponent->lockClipAndBufferPointers();
    samplePool->lockAudioFileBufferArray();
		
    AudioFileBuffer* bufferInClipComponent = clipComponent->getUnderlyingAudioClip()->getUsedBuffer();

		// find the file or directory where exactly the new component was dropped:
    x -= fileTreeComponent->getX();  
    y -= fileTreeComponent->getY();

    File dropTargetFile = fileTreeComponent->getFileAtPixelPosition(x, y);  	
		
		// copy the file into the sample-pool (into it's main folder, first - might be moved into 
		// a subfolder thereafter)
    if( samplePool->getAudioFileBufferIndexInPool(bufferInClipComponent) == -1 )
    {
      int indexInPool = samplePool->addIfNotAlreadyThere(bufferInClipComponent);
      clipComponent->assignAudioFileBuffer(samplePool->getAudioFileBuffer(indexInPool));
      //updateContents();
    }
		File fileToMove = clipComponent->getUnderlyingFile();	


    if( fileToMove == dropTargetFile )
    {
      // nothing to do
    }
    else if( dropTargetFile == File::nonexistent )
    {
			//dropTargetFile = projectSampleDirectory; // ...WTF?! after this assignment, dropTargetFile is still 0 on OSX
      samplePool->moveSampleFileInto(fileToMove, contentsList->getDirectory());  
      updateContents();
    }		
    else if( dropTargetFile.isDirectory() )
    {
      // file was dropped on some directory - move it in there:
      samplePool->moveSampleFileInto(fileToMove, dropTargetFile);  
      updateContents();
    }

    else if( dropTargetFile.existsAsFile() )
    {      
      // file was dropped onto some file - move it into its parent directory:
      dropTargetFile = dropTargetFile.getParentDirectory();
      if( dropTargetFile != fileToMove.getParentDirectory() )
        samplePool->moveSampleFileInto(fileToMove, dropTargetFile);  
			updateContents();
    }
		
    samplePool->unlockAudioFileBufferArray();	
    clipComponent->unlockClipAndBufferPointers();
    //clipComponent->unlockUnderlyingClip();
  }
}

void MixsonicProjectComponent::samplePathHasChanged(int index, const File& oldFilePath, 
                                                    const File& newFilePath)
{

}

void MixsonicProjectComponent::sampleIsToBeDeleted(int index)
{

}

void MixsonicProjectComponent::buttonClicked(Button *buttonThatWasClicked)
{
  if( buttonThatWasClicked == eraseSampleButton )
  {
    attemptToDeleteSelectedFile();
    updateContents();
  }
  else
    MixsonicBrowserBase::buttonClicked(buttonThatWasClicked);
}

void MixsonicProjectComponent::fileClicked(const File &file, const MouseEvent &e)
{
  if( e.mods.isRightButtonDown() )
    showFileContextMenu(file);
  else
    MixsonicSampleBrowser::fileClicked(file, e);
}

void MixsonicProjectComponent::updateContents()
{
  XmlElement* openness = fileTreeComponent->getOpennessState(true);
  contentsList->refresh();
	
  /*
	// a dummy-instruction loop to make sure that the DirectoryContentsList has finished loading when 
	// we draw ourselves (otherwise the tree is sometimes drawn incompletely):
	int dummy = 0;
	while( contentsList->isStillLoading() )
		dummy++;		
    // hangs up the app in windows release builds
  */
	
  fileTreeComponent->restoreOpennessState(*openness, true);
  delete openness;
	
	repaint();
}

//-------------------------------------------------------------------------------------------------
// internal functions:

bool MixsonicProjectComponent::showFileContextMenu(File fileToMove)
{
  PopupMenu menu;
  menu.addItem(1, renameStr);
  menu.addItem(2, moveButtonStr);
  menu.addItem(3, deleteStr);

  const int result = menu.show();
  if (result == 0)
    return false; // user dismissed the menu without picking anything

  // \todo the following actions in the dialog potentially render the project-file out of date, so 
  // we broadcast a changemessage for the MixsonicContentComponent to pick up:
  //sendProjectChangeMessage(this);

  if (result == 1)
    return showRenameDialog();
  else if (result == 2)
    return showMoveFileDialog();
  else if (result == 3)
    return attemptToDeleteSelectedFile();
  else
    return false;
}

bool MixsonicProjectComponent::showRenameDialog()
{
  File file = fileTreeComponent->getSelectedFile();
  if( file == File::nonexistent )
    return false;

  MixsonicEnterNameDialog *dialog 
    = new MixsonicEnterNameDialog(file.getFileName());

  Component* parent = this->getParentComponent();
  parent->addAndMakeVisible(dialog);
  dialog->setCentreRelative(0.5f, 0.5f);
  dialog->setDescriptionField(newButton->getDescriptionField());

  int result = dialog->runModalLoop();
  dialog->setVisible(false);

  if( result == 1 )
  {
    File targetFile = File( file.getParentDirectory().getFullPathName() + File::separatorString 
      + dialog->getChosenName() );
    targetFile = targetFile.withFileExtension(file.getFileExtension());
    samplePool->renameSampleFile(file, targetFile);
    updateContents();
  }
  else
  {
    // result != 1 -> user canceled the action - do nothing
  }

  // cleanup:
  parent->removeChildComponent(dialog);
  delete dialog;

  return (result != 0);
}

bool MixsonicProjectComponent::showMoveFileDialog()
{
  File file = fileTreeComponent->getSelectedFile();
  if( file == File::nonexistent )
    return false;

  MixsonicMoveFileDialog *dialog 
    = new MixsonicMoveFileDialog(samplePool->getSampleDirectory().getParentDirectory(), file);
  
  Component* parent = this->getParentComponent();
  parent->addAndMakeVisible(dialog);
  dialog->centreWithSize(400, 400);
  dialog->setDescriptionField(newButton->getDescriptionField());

  int result = dialog->runModalLoop();
  dialog->setVisible(false);

  if( result == 1 )
  {
    File targetDirectory = dialog->getDirectoryToMoveTo();
    samplePool->moveSampleFileInto(file, targetDirectory);  
    updateContents();
  }
  else
  {
    // result != 1 -> user canceled the action - do nothing
  }

  // cleanup:
  parent->removeChildComponent(dialog);
  delete dialog;

  return (result != 0);
}

bool MixsonicProjectComponent::attemptToDeleteSelectedFile()
{
  File file = fileTreeComponent->getSelectedFile();
  if( file == File::nonexistent )
    return false;

  // issue a warning box to give the user a chance to re-think this destructive operation:
  bool reallyDelete = false;
  if( file.isDirectory() )
    reallyDelete = showDirectoryDeleteWarningBox(file.getFileName());
  else
    reallyDelete = showSampleDeleteWarningBox(file.getFileName());

  // user has confirmed deletion - now do it:
  if( reallyDelete == true )
  {
    samplePool->removeSampleFromPool(file, true);  
    updateContents();
    return true;
  }
  else
    return false;
}

