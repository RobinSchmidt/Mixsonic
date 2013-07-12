#include "MixsonicContentComponent.h"



MixsonicMenuBarModel::MixsonicMenuBarModel(ApplicationCommandManager *commandManagerToUse)
{
  commandManager = commandManagerToUse;
}
const StringArray MixsonicMenuBarModel::getMenuBarNames()
{
  StringArray sa;
  sa.add("File");
  sa.add("Edit");
  sa.add("View");
  sa.add("Setup");
  return sa;
}
const PopupMenu MixsonicMenuBarModel::getMenuForIndex(int topLevelMenuIndex, const String &menuName)
{
  PopupMenu popup;
  if( menuName == "File" )
  {
    popup.addCommandItem(commandManager, MixsonicCommandManager::newProjectID);
    popup.addCommandItem(commandManager, MixsonicCommandManager::openProjectID);
    popup.addCommandItem(commandManager, MixsonicCommandManager::saveProjectID);
    popup.addCommandItem(commandManager, MixsonicCommandManager::renderArrangementID);
    popup.addCommandItem(commandManager, MixsonicCommandManager::quitID);
  }
  else if( menuName == "Edit" )
  {
    // later, we may want switch here what to cut/copy/paste/etc - clips, events, audio-regions
    popup.addCommandItem(commandManager, MixsonicCommandManager::cutClipsID);
    popup.addCommandItem(commandManager, MixsonicCommandManager::copyClipsID);
    popup.addCommandItem(commandManager, MixsonicCommandManager::pasteClipsID);
    popup.addCommandItem(commandManager, MixsonicCommandManager::duplicateClipsID);
    popup.addCommandItem(commandManager, MixsonicCommandManager::deleteClipsID);
  }
  else if( menuName == "View" )
  {
    popup.addCommandItem(commandManager, MixsonicCommandManager::toggleBrowserVisibilityID);
  }
  else if( menuName == "Setup" )
  {
    popup.addCommandItem(commandManager, MixsonicCommandManager::openSampleDirectoryDialogID);
  }
  return popup;
}
void MixsonicMenuBarModel::menuItemSelected(int menuItemID, int topLevelMenuIndex)
{
  // nothing to do - the commands are handled in MixsonicContentComponent in the perform() 
  // callback
}




//-------------------------------------------------------------------------------------------------
// construction/destruction:

MixsonicContentComponent::MixsonicContentComponent(const String &newEditorName) 
: Component(newEditorName)
, SamplePoolClient(NULL)
, drawingThread(String("DrawingThread"))
, menuBarModel(&commandManager)
{
  TimeUnitConverter::setSelectedTimeUnit(TimeUnitConverter::SECONDS);

  // initialize the application settings from the MicsonicSettings.xml file which is supposed to 
  // reside in the application directory:
  //settings.loadFromFile();

  tmpAudioClip     = NULL;
  draggedComponent = NULL;      
  browserIsVisible = true;

  // enable flashing of menubar items on command invocation by keytrokes and updating of the 
  // menubar, when commands change:
  menuBarModel.setApplicationCommandManagerToWatch(&commandManager);

  logMessage("create samplePool");
  samplePool = new SamplePool();
  //samplePool->setSampleDirectory(sampleContentDirectory); // nah - this should be the project-specific sample directory
  samplePool->addClient(this);

  // this Arrangement object will exist and be used for the entire lifetime of the application:
  logMessage("create theArrangement");
  theArrangement = new Arrangement(samplePool);

  // create the menu-bar at the top of the GUI:
  logMessage("create menuBar");
  menuBar = new MenuBarComponent(&menuBarModel);
  //menuBar->setColour(Component::colo //hmm - how am i supposed to change the colors?
  addAndMakeVisible(menuBar);

  // create the info line at the bottom of the GUI:
  logMessage("create infoLine");
  addAndMakeVisible(infoLineLabel = new RLabel(String("InfoLineLabel"), infoLineStr) );
  addAndMakeVisible(infoLineTextField = new RLabel(String("InfoLineTextField"), String::empty ));

  logMessage("create sampleBrowser");
  sampleBrowser = new MixsonicSampleBrowser(mixsonicGlobals->settings.getSampleContentDirectory());
  addAndMakeVisible(sampleBrowser );
  sampleBrowser->setDescriptionField(infoLineTextField);
  sampleBrowser->addMouseListener(this, true); // to receive its drag events
  sampleBrowser->addFileBrowserListener(this);

  // create sub-editors:
  logMessage("create projectComponent");
  projectComponent = new MixsonicProjectComponent(samplePool, getApplicationDirectory());
  addAndMakeVisible(projectComponent);
  projectComponent->setDescriptionField(infoLineTextField);
  projectComponent->addMouseListener(this, true); // to receive its drag events
  projectComponent->addFileBrowserListener(this);
  projectComponent->createDirectoryButton->addListener(this);
  projectComponent->eraseSampleButton->addListener(this);
  //projectComponent->newButton->addListener(this);
  //projectComponent->openButton->addListener(this);
  projectComponent->pauseButton->addListener(this);
  projectComponent->playButton->addListener(this);
  projectComponent->recordButton->addListener(this);
  projectComponent->rewindButton->addListener(this);
  //projectComponent->saveButton->addListener(this);


  transportController.registerObserver(this); // later: add the arrangementPlayer as observer 
                                              // instead
  transportComponent = new TransportComponent(&transportController);
  addAndMakeVisible(transportComponent);

  logMessage("create arrangementEditor");
  addAndMakeVisible(arrangementEditor = new MixsonicArrangementEditor(theArrangement, samplePool));
  arrangementEditor->setDescriptionField(infoLineTextField);
  arrangementEditor->setDrawingThread(&drawingThread);
  arrangementEditor->addChangeListener(this);

  // setup the audio stream handling:
  logMessage("set up audio stream handling");
  masterAudioSource.addInputSource(sampleBrowser,    false);
  masterAudioSource.addInputSource(projectComponent, false);
  masterAudioSource.addInputSource(theArrangement,   false);
  audioPlayer.setSource(&masterAudioSource);
  String error = deviceManager.initialise(2, 2, NULL, true);
  if( error != String::empty )
  {
    // failure to initialize the audio-device - show a dialog to select another one
  }
  deviceManager.addAudioCallback(&audioPlayer); 

  //------------------------------------------------
  // debug/test:
  //deviceManager.restartLastAudioDevice(); // doesn't help
  //audioPlayer.
  const OwnedArray<AudioIODeviceType>& deviceTypes = deviceManager.getAvailableDeviceTypes();
  for(int i = 0; i < deviceTypes.size(); i++)
  {
    AudioIODeviceType *deviceType = deviceTypes[i];
    int dummy = 0;
  }
  AudioIODevice* currentDevice = deviceManager.getCurrentAudioDevice();
  AudioDeviceManager::AudioDeviceSetup deviceSetup;
  deviceManager.getAudioDeviceSetup(deviceSetup);
  //-----------------------------------------------------



  logMessage("create temporary project");
  createTemporaryProject();
  setCurrentDirectory(projectDirectory);

  setSize(800, 600);

  logMessage("start drawing thread");
  drawingThread.startThread(3); // low priority for redrawing     
}

MixsonicContentComponent::~MixsonicContentComponent()
{
  drawingThread.stopThread(2000);

  deviceManager.removeAudioCallback(&audioPlayer); 

  audioPlayer.setSource(NULL);
  deleteAllChildren();
  delete theArrangement;

  // we must remove ourselves as client before the samplePool is deleted and set our pointer to   
  // NULL to avoid access violation in destructor of our baseclass SamplePoolClient
  samplePool->removeClient(this); 
  delete samplePool;
  samplePool = NULL;

  clearTemporaryProject();
}

//-------------------------------------------------------------------------------------------------
// inquiry:

String MixsonicContentComponent::getProjectName()
{
  return projectFile.getFileNameWithoutExtension();
}

//-------------------------------------------------------------------------------------------------
// callbacks:

void MixsonicContentComponent::mouseUp(const MouseEvent &e)
{
  if( draggedComponent != NULL )
  {
    // the MouseEvent may originate from some child component, so we first convert it into our own
    // coordinate system:
    MouseEvent e2 = e.getEventRelativeTo(this);

    bool componentWasTakenOver  = false;

    if( isInsideArrangementEditor(e2.x, e2.y) )
    {
      // when dropping a clip onto the arrangement, we possibly have to copy the file into the 
      // sample-pool before and re-assign the new file to the clip:

      //draggedComponent->lockUsedBufferPointer();
      draggedComponent->lockClipAndBufferPointers();
      samplePool->lockAudioFileBufferArray();

      AudioFileBuffer* bufferInDraggedComponent = draggedComponent->getUsedBuffer();
      int indexInPool = samplePool->getAudioFileBufferIndexInPool(bufferInDraggedComponent);
      if( indexInPool == -1 )
      {
        indexInPool = samplePool->addIfNotAlreadyThere(bufferInDraggedComponent);
        projectComponent->updateContents();
      }
      draggedComponent->assignAudioFileBuffer(samplePool->getAudioFileBuffer(indexInPool));

      samplePool->unlockAudioFileBufferArray();
      draggedComponent->unlockClipAndBufferPointers();
      //draggedComponent->unlockUsedBufferPointer();

      // obtain a MouseEvent with respect to the arrangementPanel child-component and callback the 
      // arrangementPanel with the dropped component when it was dropped onto it:
      MouseEvent e3 = e.getEventRelativeTo(arrangementEditor);
      MouseEvent e4 = e.getEventRelativeTo(draggedComponent);
      componentWasTakenOver = arrangementEditor->dropComponent(draggedComponent, e3.x-e4.x, e3.y);
    }
    else if( isInsideProjectComponent(e2.x, e2.y) )
    {
      // we are dropping the clip into the sample-pool:
      MouseEvent e3 = e.getEventRelativeTo(projectComponent);
      MouseEvent e4 = e.getEventRelativeTo(draggedComponent);
      projectComponent->dropComponent(draggedComponent, e3.x-e4.x, e3.y); 
      componentWasTakenOver = false;
    }

    // either the embedded arrangementEditor has taken over the temporary clip component (and its 
    // underlying clip) in which case we only clear our references to the objects, or has not taken
    // over, in which case we also delete the objects:
    removeChildComponent(draggedComponent);
    if( componentWasTakenOver == false )
    {
      delete draggedComponent;
      delete tmpAudioClip;
    }
    tmpAudioClip     = NULL;
    draggedComponent = NULL; 
  }
}

void MixsonicContentComponent::mouseDrag(const MouseEvent &e)
{
  if( draggedComponent != NULL )
    componentDragger.dragComponent(draggedComponent, e, NULL);
}

void MixsonicContentComponent::buttonClicked(Button *buttonThatWasClicked)
{
  if( buttonThatWasClicked == projectComponent->createDirectoryButton )
    showCreateDirectoryDialog();
  else if( buttonThatWasClicked == projectComponent->recordButton )
    showRecordDialog();
  /*
  // obsolete:
  else if(   buttonThatWasClicked == arrangementEditor->playButton 
          || buttonThatWasClicked == arrangementEditor->pauseButton )
  {
    bool play =     arrangementEditor->playButton->getToggleState() 
                && !arrangementEditor->pauseButton->getToggleState();

    double sampleRate = deviceManager.getCurrentAudioDevice()->getCurrentSampleRate();
    int    bufferSize = deviceManager.getCurrentAudioDevice()->getCurrentBufferSizeSamples();
    GlobalTimeFrameComponent::setSystemSampleRate(sampleRate);
    if( play == true )
      arrangementEditor->startPlayback(bufferSize, sampleRate);
    else
      arrangementEditor->stopPlayback();
  }

  */
}

void MixsonicContentComponent::changeListenerCallback(ChangeBroadcaster *objectThatHasChanged)
{

  if( objectThatHasChanged == arrangementEditor )
    setSongFileUpToDateFlag(false);
  else
  {
    jassertfalse;  // arrangementEditor should currently be the only object, we receive 
                   // change-callbacks from
  }
}

void MixsonicContentComponent::selectionChanged()
{  

}

void MixsonicContentComponent::fileClicked(const File &file, const MouseEvent &e)
{
  File f;
  if( sampleBrowser->isParentOf(e.eventComponent) )
    f = sampleBrowser->getSelectedFile();
  else if( projectComponent->isParentOf(e.eventComponent) )
    f = projectComponent->getSelectedFile();
  createTemporaryClipComponentAt(e, f);
}

void MixsonicContentComponent::fileDoubleClicked(const File &file)
{

}

void MixsonicContentComponent::browserRootChanged(const File &newRoot)
{

}

void MixsonicContentComponent::samplePathHasChanged(int index, const File &oldFilePath, 
                                                    const File &newFilePath)
{
  setSongFileUpToDateFlag(false);
}

void MixsonicContentComponent::sampleIsToBeDeleted(int index)
{
  setSongFileUpToDateFlag(false);
}


  
ApplicationCommandTarget* MixsonicContentComponent::getNextCommandTarget()
{
  return nullptr;
}

void MixsonicContentComponent::getAllCommands(Array<CommandID> &commands)
{
  // our command IDs are just the default enum-indices starting at index 1 and we handle them all: 
  commands.clear(); // probably not necessary
  commands.ensureStorageAllocated(MixsonicCommandManager::numCommandIDsPlus1-1);
  for(int i = 0; i < MixsonicCommandManager::numCommandIDsPlus1-1; i++)
    commands.add(i+1);
}
 
void MixsonicContentComponent::getCommandInfo(CommandID commandID, ApplicationCommandInfo &result)
{
  result = *commandManager.getCommandForID(commandID);
  if( commandID == MixsonicCommandManager::toggleBrowserVisibilityID )
    result.setTicked(browserIsVisible);
}
 
bool MixsonicContentComponent::perform(const InvocationInfo &info)
{
  // this is all very preliminary - we should refactor to a proper MVC architechture and
  // handle the commands there. the editors should update themselves by receiving callbacks from
  // the controller-classes

  bool commandWasHandled = true;
  switch( info.commandID )
  {
    // File:
  case MixsonicCommandManager::newProjectID:
    {
      transportController.stopPlayback();
      if( !projectFileIsUpToDate )
      {
        if( showSongUnsavedWarningBox() == true ) // user must confirm to dismiss unsaved projectFile
          showNewProjectDialog();
      }
      else
        showNewProjectDialog();    
    } break;
  case MixsonicCommandManager::openProjectID:
    {
      transportController.stopPlayback();
      if( !projectFileIsUpToDate )
      {
        if( showSongUnsavedWarningBox() == true )
          openLoadingDialog();
      }
      else
        openLoadingDialog();
      // \todo get rid of the code-duplication
    } break;
  case MixsonicCommandManager::saveProjectID:       saveProject();      break;
  case MixsonicCommandManager::renderArrangementID: showExportDialog(); break;
  case MixsonicCommandManager::quitID:
    {
      if( !projectFileIsUpToDate )
      {
        if( showSongUnsavedWarningBox() == true )
          JUCEApplication::quit();   
      }   
      else
        JUCEApplication::quit();  
      // \todo this code was copy/pasted from MixsonicWindow::closeButtonPressed - get rid of this
      // code duplication
    } break;

    // Edit:
  case MixsonicCommandManager::cutClipsID:
    {
      arrangementEditor->arrangementPanel->copySelectedClipsIntoClipboard();
      arrangementEditor->arrangementPanel->deleteSelectedClips();
    } break;
  case MixsonicCommandManager::copyClipsID:
    {
      arrangementEditor->arrangementPanel->copySelectedClipsIntoClipboard();
    } break;
  case MixsonicCommandManager::pasteClipsID:
    {
      arrangementEditor->arrangementPanel->pasteClipsFromClipboard();
      arrangementEditor->adjustArrangementLength();
    } break;
  case MixsonicCommandManager::duplicateClipsID:
    {
      arrangementEditor->arrangementPanel->copySelectedClipsIntoClipboard();
      arrangementEditor->arrangementPanel->pasteClipsFromClipboard(
        arrangementEditor->arrangementPanel->arrangementToEdit->getLastClipEndTimeInBeats(true));
      arrangementEditor->adjustArrangementLength();
    } break;
  case MixsonicCommandManager::deleteClipsID:
    {
      arrangementEditor->arrangementPanel->deleteSelectedClips();
    } break;
  case MixsonicCommandManager::splitClipsID:
    {
      jassertfalse; // not yet implemented
    } break;

    // View:
  case MixsonicCommandManager::toggleBrowserVisibilityID:
    {
      browserIsVisible = !browserIsVisible;
      resized(); // will set up bounds of child-components - it would be cleaner to have a function
                 // "setupChildComponents" or something because actually there was no resize
    } break;

    // Setup:
  case MixsonicCommandManager::openSampleDirectoryDialogID: showGlobalSettingsDialog(); break;


  default:
    {
      commandWasHandled = false;
    }
  }
  return commandWasHandled;
}
 
void MixsonicContentComponent::playbackStarted()
{
  double sampleRate = deviceManager.getCurrentAudioDevice()->getCurrentSampleRate();
  int    bufferSize = deviceManager.getCurrentAudioDevice()->getCurrentBufferSizeSamples();

  // for debug:
  //deviceManager.playTestSound();

  GlobalTimeFrameComponent::setSystemSampleRate(sampleRate);
  arrangementEditor->startPlayback(bufferSize, sampleRate);
}
 
void MixsonicContentComponent::playbackStopped()
{  
  arrangementEditor->stopPlayback();
}
 
void MixsonicContentComponent::wasRewindedToStart()
{
  jassertfalse; // not yet implemented
}

//-------------------------------------------------------------------------------------------------
// state management:

XmlElement* MixsonicContentComponent::getStateAsXml() const
{
  XmlElement* xmlState = new XmlElement(String("MIXSONICSONG")); 

  // add the state of the arrangement as child element:  
  XmlElement* arrangementState = arrangementEditor->getStateAsXml(getProjectDirectory());  
  xmlState->addChildElement(arrangementState);

  return xmlState;
}

void MixsonicContentComponent::setStateFromXml(const XmlElement &xmlState)
{
  XmlElement* childState = xmlState.getChildByName("ARRANGEMENT"); 
  if( childState != NULL )
    arrangementEditor->setStateFromXml(*childState, getProjectDirectory());

  // update the global widgets (master volume, bpm, etc.):
  projectComponent->updateContents();
}

bool MixsonicContentComponent::loadStateFromXmlFile(const File& fileToLoadFrom)
{
	clearTemporaryProject(); 
  projectIsTemporary = false;
  setProjectDirectory(fileToLoadFrom.getParentDirectory());
  if( StateFileManager::loadStateFromXmlFile(fileToLoadFrom) == true )
  {
    projectFile = fileToLoadFrom;
    setSongFileUpToDateFlag(true);
    clearTemporaryProject();
    sendChangeMessage();
    return true;
  }
  else
    return false;
}

bool MixsonicContentComponent::saveStateToXmlFile(const File& fileToSaveTo)
{
  projectIsTemporary = false;
  if( fileToSaveTo != File::nonexistent && StateFileManager::saveStateToXmlFile(fileToSaveTo) == true )
  {
    projectFile = fileToSaveTo;
    setSongFileUpToDateFlag(true);
    return true;
  }
  else
  {
    showFileCouldNotBeSavedBox(fileToSaveTo.getFileName());
    return false;
  }
}

//-------------------------------------------------------------------------------------------------
// appearance stuff:

void MixsonicContentComponent::resized()
{
  int x = 0;
  int y = 0;
  int w = getWidth();
  int h = getHeight();

  // menubar and infoline:
  menuBar->setBounds(0, 0, getWidth(), 24);
  infoLineLabel->setBounds(       4, h-20,    88, 16);
  infoLineTextField->setBounds(88+4, h-20,  w-88, 16); 

  // browser:
  if( browserIsVisible )
  {
    y = menuBar->getBottom();
    w = jmin(w/4, 200);
    sampleBrowser->setBounds(x, y, w, h/2);
    y = sampleBrowser->getBottom();
    h = infoLineLabel->getY() - y;
    projectComponent->setBounds(x, y, w, h);
  }
  else
  {
    sampleBrowser->setBounds(0, 0, 0, 0);
    projectComponent->setBounds(0, 0, 0, 0);
  }

  // transport-section:
  //x = sampleBrowser->getRight() + MixsonicArrangementEditor::trackControlComponentWidth;
  x = sampleBrowser->getRight();
  y = menuBar->getBottom();
  h = 40;
  w = transportComponent->getRequiredWidth(h);
  transportComponent->setBounds(x, y, w, h);

  // arrangement editor:
  x = sampleBrowser->getRight();
  y = transportComponent->getBottom();
  h = infoLineLabel->getY() - y;
  w = getWidth() - x;
  arrangementEditor->setBounds(x, y, w, h);
}

void MixsonicContentComponent::paint(Graphics &g)
{
  g.fillAll(backgroundColor);
  g.setColour(outlineColor);
  //g.drawRect(1.f, 1.f, (float)getWidth()-2.f, (float)getHeight()-2.f, 2.f);
}

//-------------------------------------------------------------------------------------------------
// others:

void MixsonicContentComponent::initializeSong()
{
  setSongFileUpToDateFlag(true);

  // initialize the arrangement (and its GUI):  
  arrangementEditor->initializeSong();
  projectComponent->updateContents();
}

//-------------------------------------------------------------------------------------------------
// internal functions:

void MixsonicContentComponent::showExportDialog()
{
  // stop playing (if it currently does) and remember the current position of the time-cursor:
  transportController.stopPlayback();
  int64 currentPosition = theArrangement->getNextReadPosition();

  // show the rendering dialog:
  File exportedFile = projectFile.withFileExtension(String("wav"));

  // create the modal dialog:
  MixsonicAudioExportDialog *dialog = new MixsonicAudioExportDialog(theArrangement, exportedFile);
  addAndMakeVisible(dialog);
  dialog->setCentreRelative(0.5f, 0.5f);
  dialog->setDescriptionField(infoLineTextField);

  // run the dialog in a modal loop until it returns:
  dialog->runModalLoop(); 

  // cleanup:
  removeChildComponent(dialog);
  delete dialog;

  // restore the position of the time cursor (rendering may have modified it):
  theArrangement->setNextReadPosition(currentPosition);
}

void MixsonicContentComponent::showRecordDialog()
{
  // stop playing (if it currently does):
  transportController.stopPlayback();

  // create a default target-file which will be used to write into when the user does not specify
  // a custom target file:
  File parentDir = projectComponent->getCurrentDirectory();
  if( !parentDir.isDirectory() )
    parentDir = parentDir.getParentDirectory();
  String targetPath = parentDir.getFullPathName() + File::separatorString 
    + String("ExportedAudio.wav"); // \todo use projectName + "Exported", and/or store file in "Exported" directory
  File targetFile = File(targetPath).getNonexistentSibling();

  // create the modal dialog:
  MixsonicAudioRecordDialog *dialog = new MixsonicAudioRecordDialog(&deviceManager, targetFile);
  addAndMakeVisible(dialog);
  dialog->setCentreRelative(0.5f, 0.5f);
  dialog->setDescriptionField(infoLineTextField);

  // set the callback object of the AudioDeviceManager to this dialog (which is also subclassed 
  // from AudioIODeviceCallback:
  deviceManager.removeAudioCallback(&audioPlayer);
  deviceManager.addAudioCallback(dialog);

  // run the dialog in a modal loop until it returns:
  dialog->runModalLoop();

  // set the callback object of the AudioDeviceManager back to the audioPlayer member:
  deviceManager.removeAudioCallback(dialog);
  deviceManager.addAudioCallback(&audioPlayer);

  // cleanup:
  removeChildComponent(dialog);
  delete dialog;

  // update the project panel:
  projectComponent->updateContents();
}

void MixsonicContentComponent::showNewProjectDialog()
{
 
  MixsonicNewProjectDialog *dialog 
    = new MixsonicNewProjectDialog(mixsonicGlobals->settings.getProjectsParentDirectory());
  addAndMakeVisible(dialog);
  dialog->setCentreRelative(0.5f, 0.5f);
  dialog->setDescriptionField(infoLineTextField);
  dialog->grabKeyboardFocus();
  
  bool test = dialog->hasKeyboardFocus(true); 
    // mmm... indicates that is should have the focus, but apparently doesn't

  int result = dialog->runModalLoop();
  test = dialog->hasKeyboardFocus(true);

  dialog->setVisible(false);
  if( result == 1 )
  {
    // user chose to create a new project...
    clearTemporaryProject();
    String newProjectName = dialog->getChosenNewProjectName();
    if( createNewProject(newProjectName, false) == false )
      showProjectCouldNotBeCreatedBox(newProjectName);
  }
  else
  {
    // result != 1 -> user canceled the action - do nothing
  }

  // cleanup:
  removeChildComponent(dialog);
  delete dialog;


  /*
  MixsonicNewProjectDialog dialog(projectsParentDirectory);
  DialogWindow newProjectWindow(String::empty, Colours::white, true, false);
  newProjectWindow.setUsingNativeTitleBar(true); // ....but it still uses the JUCE-titlebar
  newProjectWindow.showModalDialog(String::empty, &dialog, this, 
    Colours::white, true, false, false);
  */
}

void MixsonicContentComponent::showCreateDirectoryDialog()
{
  if( !projectSampleDirectory.isDirectory() )
  {
    showFileIsNoDirectoryErrorBox(projectSampleDirectory);
    return;
  }

  File rootDir = projectComponent->getSelectedFile();
  if( rootDir == File::nonexistent || !rootDir.isDirectory() )
    rootDir = projectSampleDirectory;
	
	// some debug-tests:
	/*
	bool nonExistent = rootDir == File::nonexistent;
	bool isDirectory = rootDir.isDirectory();
	if( nonExistent )
		jassertfalse;
	if( isDirectory )
		jassertfalse;
	*/
	
  MixsonicCreateDirectoryDialog *dialog 
    = new MixsonicCreateDirectoryDialog(rootDir);

  addAndMakeVisible(dialog);
  dialog->setCentreRelative(0.5f, 0.5f);
  dialog->setDescriptionField(infoLineTextField);

  int result = dialog->runModalLoop();
  dialog->setVisible(false);
  if( result == 1 )
  {
    // user chose to create a new project...
    File newDirectory = dialog->getDirectoryToCreate();

    //if( newDirectory.createDirectory() == true )  // juce 1.46
    if( newDirectory.createDirectory() == Result::ok() )
      projectComponent->updateContents();
    else
      showDirectoryCouldNotBeCreatedBox(newDirectory);
  }
  else
  {
    // result != 1 -> user canceled the action - do nothing
  }

  // cleanup:
  removeChildComponent(dialog);
  delete dialog;
}

void MixsonicContentComponent::showGlobalSettingsDialog()
{
  MixsonicGlobalSettingsDialog *dialog = 
    new MixsonicGlobalSettingsDialog(&mixsonicGlobals->settings);
  addAndMakeVisible(dialog);
  dialog->setCentreRelative(0.5f, 0.5f);
  dialog->setDescriptionField(infoLineTextField);

  int result = dialog->runModalLoop();
  dialog->setVisible(false);
  if( result == 1 ) // otherwise: user canceled the action -> do nothing
    sampleBrowser->setRootDirectory(mixsonicGlobals->settings.getSampleContentDirectory());


  jassertfalse; // \todo: instead of retrieving settings here, we should use callbacks like
                // sampleDirectoryChanged, pluginDirectoriesChanged, etc.


  // cleanup:
  removeChildComponent(dialog);
  delete dialog;
}

void MixsonicContentComponent::createTemporaryProject()
{
  //createNewProject(tempProjectNameStr, true); 
	  // the globally defined string seems to messed up on OSX
	createNewProject(String("Temp"), true); 
}

void MixsonicContentComponent::clearTemporaryProject()
{
  if( projectIsTemporary ) // make sure to not accidently delete a non-temporary project
  {
		if( projectDirectory.exists() )
		{
			projectDirectory.setReadOnly(false, true);
			projectDirectory.deleteRecursively();
      projectDirectory.deleteFile(); // additionaly required, if the 'Temp' folder was not empty
                                     // ...but why? 
		}
    projectDirectory = File::nonexistent;
    projectFile      = File::nonexistent;
  }
}

void MixsonicContentComponent::saveProject()
{
  if( projectIsTemporary )
  {
    // factor out into a function:
    MixsonicEnterProjectNameDialog *dialog 
      = new MixsonicEnterProjectNameDialog(projectDirectory.getParentDirectory());

    addAndMakeVisible(dialog);
    dialog->setCentreRelative(0.5f, 0.5f);
    dialog->setDescriptionField(infoLineTextField);

    int result = dialog->runModalLoop();
    dialog->setVisible(false);
    if( result == 1 )
    {
      String newProjectName = dialog->getChosenNewProjectName();
      String newPath 
        = projectDirectory.getParentDirectory().getFullPathName() 
        + File::separatorString + newProjectName;
      File newProjectDirectory = File(newPath);
      if( !projectDirectory.moveFileTo(newProjectDirectory) )
      {
        showFileCouldNotBeRenamedBox(projectDirectory.getFileName(), 
          newProjectDirectory.getFileName());
      }
      else
        projectDirectory = newProjectDirectory;
      projectFile = File(projectDirectory.getFullPathName() + File::separatorString 
        + newProjectName + String(".xml"));
      setProjectDirectory(projectDirectory);
      saveStateToXmlFile(projectFile);
      sendChangeMessage();
    }
    else
    {
      // result != 1 -> user canceled the action - do nothing
    }

    // cleanup:
    removeChildComponent(dialog);
    delete dialog;
  }
  else
    saveStateToXmlFile(projectFile);
  projectIsTemporary = false;
}

void MixsonicContentComponent::setSongFileUpToDateFlag(bool isUpToDate)
{
  projectFileIsUpToDate = isUpToDate;
}

void MixsonicContentComponent::setProjectDirectory(const File& newProjectDirectory)
{
  projectDirectory = newProjectDirectory;
  StateFileManager::setCurrentDirectory(projectDirectory);

  // create a sample subdirectory, if not already there:
  projectSampleDirectory = File(newProjectDirectory.getFullPathName() 
    + File::separatorString + String("Samples"));
  if( !projectSampleDirectory.exists() )
  {
    if( !projectSampleDirectory.createDirectory() )
      showDirectoryCouldNotBeCreatedBox(projectSampleDirectory);
  }
  samplePool->setSampleDirectory(projectSampleDirectory);
  projectComponent->setRootDirectory(projectSampleDirectory);
}

File MixsonicContentComponent::getProjectDirectory() const
{ 
  return  projectDirectory;
}

bool MixsonicContentComponent::createNewProject(const String& newProjectName, 
                                                bool newProjectIsTemporary)
{
  logMessage("MixsonicContentComponent::createNewProject");

  String newProjectPath = mixsonicGlobals->settings.getProjectsParentDirectory().getFullPathName() 
    + File::separatorString + newProjectName;
  projectDirectory = File(newProjectPath).getNonexistentSibling();
  if( !projectDirectory.createDirectory() )
  {
    showDirectoryCouldNotBeCreatedBox(projectDirectory);
    return false;
  }
  setProjectDirectory(projectDirectory);

  projectSampleDirectory = File( projectDirectory.getFullPathName() + File::separatorString 
    + String("Samples") );
  if( !projectSampleDirectory.createDirectory() )
  {
    showDirectoryCouldNotBeCreatedBox(projectSampleDirectory);
    return false;
  }

  projectFile = File( projectDirectory.getFullPathName() + File::separatorString 
    + projectDirectory.getFileName() + String(".xml") );

  logMessage("initializeSong()");
  initializeSong();      
  projectFileIsUpToDate = true;
  projectIsTemporary    = newProjectIsTemporary;

  logMessage("sendChangeMessage()");
  sendChangeMessage(); // who receives this?

  return true;
}

bool MixsonicContentComponent::isInsideBrowserComponent(int x, int y) const
{
  if(  x >= sampleBrowser->getX() && x <= sampleBrowser->getRight()
    && y >= sampleBrowser->getY() && y <= sampleBrowser->getBottom() )
  {
    return true;
  }
  else
    return false;
}

bool MixsonicContentComponent::isInsideProjectComponent(int x, int y) const
{
  if(  x >= projectComponent->getX() && x <= projectComponent->getRight()
    && y >= projectComponent->getY() && y <= projectComponent->getBottom() )
  {
    return true;
  }
  else
    return false;
}

bool MixsonicContentComponent::isInsideArrangementEditor(int x, int y) const
{
  if(  x >= arrangementEditor->getX() && x <= arrangementEditor->getRight()
    && y >= arrangementEditor->getY() && y <= arrangementEditor->getBottom()-32 )
  {
    return true;
  }
  else
    return false;
}

void MixsonicContentComponent::createTemporaryClipComponentAt(const MouseEvent& e, const File& f)
{
  if( draggedComponent != NULL )
  {
    jassertfalse;
     // there seems to be already some component assigned to this pointer - this should not be 
     // the case when invoking this function.
    delete draggedComponent;
    draggedComponent = NULL;
  }

  if( draggedComponent == NULL )
  {
    tmpAudioFileBuffer.loadAudioDataFromFile(f, false);
    if( tmpAudioFileBuffer.isAudioFileValid() )
    {
      MouseEvent e2 = e.getEventRelativeTo(this); 

      tmpAudioClip                         = new AudioClip(&tmpAudioFileBuffer);
      AudioClipComponent* newClipComponent = new AudioClipComponent(tmpAudioClip);
      //newClipComponent->setSelected(true, false, false); // new since switching from 1.46
      draggedComponent                     = newClipComponent;
      draggedComponent->setDrawingThread(&drawingThread);
      newClipComponent->setDescriptionField(infoLineTextField);

      //draggedComponent->lockUnderlyingClip();
      draggedComponent->lockClipAndBufferPointers();

      int w = arrangementEditor->getDesiredClipWidth(newClipComponent->getUnderlyingAudioClip());
      //int h = TrackComponent::getDesiredHeight(); // old
      int h = arrangementEditor->arrangementPanel->getTrackHeight();
      int x = e2.getMouseDownX() - w/2; // not necessarily equal to e2.x - why?!
      int y = e2.getMouseDownY() - h/2;
      draggedComponent->setBounds(x, y, w, h);


      draggedComponent->unlockClipAndBufferPointers();
      //draggedComponent->unlockUnderlyingClip();

      addAndMakeVisible(draggedComponent);           // will be removed/deleted in mouseUp

      // let us receive the mouseDrag callback here:
      draggedComponent->addMouseListener(this, true);

      componentDragger.startDraggingComponent(draggedComponent, e2);
    }
  }
}
