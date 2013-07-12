#include "PluginComponents.h"

AudioProcessorEditorContainer::AudioProcessorEditorContainer(AudioProcessorEditor *editorToWrap,  
  bool shouldTakeOwnership, DeletionManager *deletor, Activatable *ownerToWatchForActivation) 
: editor(editorToWrap)
, DeletionRequester(deletor)
{
  titleBarHeight = 18;
  ownsEditor     = shouldTakeOwnership;

  addAndMakeVisible(editor);

  visibilityController = ownerToWatchForActivation;
  if( visibilityController != nullptr )
    visibilityController->registerActivationObserver(this);

  addAndMakeVisible( nameLabel = new RLabel() );
  nameLabel->setText(editor->getAudioProcessor()->getName(), false);
  //nameLabel->setColour(Label::outlineColourId, outlineColor);
  //nameLabel->setColour(Label::backgroundColourId, backgroundColor);
  //nameLabel->setColour(Label::textColourId, textColor);
  nameLabel->addMouseListener(this, false);

  addAndMakeVisible( closeButton = new RButton("X") );
  closeButton->setDescription("Close pugin editor");
  closeButton->addListener(this);

  setSize(editor->getWidth(), editor->getHeight() + titleBarHeight);
}

AudioProcessorEditorContainer::~AudioProcessorEditorContainer()
{
  if( visibilityController != nullptr )
    visibilityController->deregisterActivationObserver(this);

  removeChildComponent(editor);
  if( ownsEditor )
    delete editor;

  deleteAllChildren(); 
}

// setup:  

void AudioProcessorEditorContainer::showInFrontOfAppWindow()
{
  setVisible(true);
  toFront(true);
}

void AudioProcessorEditorContainer::childBoundsChanged(Component *child)
{
  if( child == editor )
    setSize(editor->getWidth(), editor->getHeight() + titleBarHeight);
}

void AudioProcessorEditorContainer::resized()
{
  if( editor != nullptr )
    editor->setTopLeftPosition(0, titleBarHeight);

  int labelHeight = titleBarHeight;
  int buttonSize  = titleBarHeight;

  nameLabel->setBounds(0, 0, getWidth()-buttonSize+1, labelHeight);
  closeButton->setBounds(getWidth()-buttonSize, 0, buttonSize, buttonSize);

  // \todo set up preset load/save widgets
}

void AudioProcessorEditorContainer::paint(Graphics &g)
{
  g.setColour(Colours::black); // preliminary
  g.fillRect(0, 0, getWidth(), titleBarHeight);
}

void AudioProcessorEditorContainer::activationStatusChanged(Activatable *activatable, 
                                                            bool isActive)
{
  setVisible(isActive);
}

void AudioProcessorEditorContainer::buttonClicked(Button* button)
{
  if( button == closeButton )
    requestDeletion();
}

void AudioProcessorEditorContainer::mouseDown(const MouseEvent &e)
{ 
  dragger.startDraggingComponent(this, e);
}
 
void AudioProcessorEditorContainer::mouseDrag(const MouseEvent &e)
{
  dragger.dragComponent(this, e, nullptr);
}

//=================================================================================================
// class AudioPluginSlotComponent:

AudioPluginSlotComponent::AudioPluginSlotComponent(PluginSlot *pluginSlotToEdit)
{
  jassert(pluginSlotToEdit != nullptr);

  slotToEdit = pluginSlotToEdit;
  slotToEdit->enterLock();
  if( slotToEdit != nullptr )
    slotToEdit->addChangeListener(this);
  slotToEdit->exitLock();

  customEditor    = nullptr;
  parameterEditor = nullptr;
  knownPluginList = nullptr;
  slotIsRemovable = true;

  addAndMakeVisible( nameLabel = new RLabel() );
  nameLabel->setColour(Label::outlineColourId, outlineColor);
  nameLabel->setColour(Label::backgroundColourId, backgroundColor);
  nameLabel->setColour(Label::textColourId, textColor);
  nameLabel->addMouseListener(this, false);

  updateLabelText();

  setKnownPluginList(&mixsonicGlobals->knownPlugins);
}

AudioPluginSlotComponent::~AudioPluginSlotComponent()
{
  delete customEditor;
  delete parameterEditor;

  slotToEdit->enterLock();
  slotToEdit->removeChangeListener(this); 
  slotToEdit->exitLock();

  deleteAllChildren();
}

// setup:

void AudioPluginSlotComponent::setBypass(bool shouldBeBypassed)
{
  slotToEdit->setBypass(shouldBeBypassed);
  repaint();
}

void AudioPluginSlotComponent::setKnownPluginList(KnownPluginList *listToUse)
{
  knownPluginList = listToUse;
}

// inquiry:

bool AudioPluginSlotComponent::isEmpty()
{
  ScopedLock lock(*slotToEdit->getMutex());
  return slotToEdit->isEmpty();
}

bool AudioPluginSlotComponent::isCustomEditorOpen()
{
  return customEditor != nullptr;
}

bool AudioPluginSlotComponent::isParameterEditorOpen()
{
  return parameterEditor != nullptr;
}

bool AudioPluginSlotComponent::isAnyEditorOpen()
{
  return isCustomEditorOpen() || isParameterEditorOpen();
}

// callbacks:

void AudioPluginSlotComponent::changeListenerCallback(ChangeBroadcaster *source)
{
  updateLabelText();
}
 
void AudioPluginSlotComponent::mouseDown(const MouseEvent &e)
{
  if( e.mods.isLeftButtonDown() )
  {
    if( isEmpty() )
      openPopUpMenu();
    else
    {
      if( !isAnyEditorOpen() )
        openEditor();
      else
        closeEditors();
    }
  }
  else if( e.mods.isRightButtonDown() )
    openPopUpMenu();
}

void AudioPluginSlotComponent::resized()
{
  nameLabel->setBounds(0, 0, getWidth(), getHeight());
}

void AudioPluginSlotComponent::paintOverChildren(Graphics &g)
{
  if( slotToEdit->isBypassed() )
  {
    g.setColour(outlineColor);
    g.drawLine(0.f, 0.f, (float) getWidth(), (float) getHeight(), 1.f);
    g.drawLine(0.f, (float) getHeight(), (float) getWidth(), 0.f, 1.f);
      // maybe draw a cross-hatch pattern instead
  }
}

void AudioPluginSlotComponent::handleDeletionRequest(DeletionRequester *objectThatWantsToBeDeleted)
{
  if( objectThatWantsToBeDeleted == customEditor )
    closeCustomEditor();
  else if( objectThatWantsToBeDeleted == parameterEditor )
    closeParameterEditor();
}

// misc:

void AudioPluginSlotComponent::updateLabelText()
{
  ScopedLock lock(*slotToEdit->getMutex());
  if( isEmpty() )
    nameLabel->setText("", false);
  else
    nameLabel->setText(slotToEdit->getPlugin()->getName(), false);
}

// internal:

void AudioPluginSlotComponent::openPopUpMenu()
{
  PopupMenu menu;

  if( !isEmpty() )
  {
    slotToEdit->enterLock();
    menu.addItem(1, "Bypass", true, slotToEdit->isBypassed());
    menu.addItem(2, "Show Parameters"); 
    menu.addItem(3, "Open GUI", slotToEdit->getPlugin()->hasEditor()); 
    menu.addItem(4, "Remove Plugin"); 
    menu.addSeparator();
    slotToEdit->exitLock();
  }
  if( slotIsRemovable ) 
    menu.addItem(5, "Remove Slot"); 
  menu.addItem(6, "Load Plugin");

  if( knownPluginList != nullptr )
    knownPluginList->addToMenu(menu, KnownPluginList::sortByFileSystemLocation);

  const int result = menu.show();

  switch( result )
  {
  case 0: /* user dismissed the menu */         break;
  case 1: setBypass(!slotToEdit->isBypassed()); break;
  case 2: openParameterEditor();                break;
  case 3: openCustomEditor();                   break;
  case 4: removePlugin();                       break;
  case 5: requestDeletion();                    break;
  case 6: openLoadPluginDialog();               break;
  default:
    {
      if( knownPluginList != nullptr )
      {
        int listIndex = knownPluginList->getIndexChosenByMenu(result);
        PluginDescription* description = knownPluginList->getType(listIndex);
        loadPluginFromDescription(description);
      }
    }
  }
}

void AudioPluginSlotComponent::openLoadPluginDialog()
{
  File startDirectory = getApplicationDirectory(); 
    // later use a user-specified plugin folder stored in the global preferences

  FileChooser chooser("Select Plugin", startDirectory, "*.dll"); 
    // later use a system-specific extension - write a function getPluginFileExtensions()

  if( chooser.browseForFileToOpen() )
    loadPluginFromFile(chooser.getResult());
}

void AudioPluginSlotComponent::loadPluginFromFile(const File& pluginFile)
{ 
  closeEditors();
  slotToEdit->loadPlugin(pluginFile);
  openEditor();
}

void AudioPluginSlotComponent::loadPluginFromDescription(const PluginDescription* description)
{
  closeEditors();
  slotToEdit->loadPlugin(description);
  openEditor();        
}

void AudioPluginSlotComponent::removePlugin()
{
  closeEditors();
  slotToEdit->setPlugin(nullptr, true);
}

void AudioPluginSlotComponent::openEditor()
{
  ScopedLock lock(*slotToEdit->getMutex());
  jassert(!isEmpty());
  if( slotToEdit->getPlugin()->hasEditor() )
    openCustomEditor();
  else
    openParameterEditor();
}

void AudioPluginSlotComponent::openCustomEditor()
{
  ScopedLock lock(*slotToEdit->getMutex());
  if( customEditor != nullptr )
  {
    customEditor->showInFrontOfAppWindow();
    return; 
  }
  AudioProcessorEditor *pluginEditor = slotToEdit->getPlugin()->createEditorIfNeeded();
  if( pluginEditor != nullptr )
    wrapPluginEditorIntoContainerAndShow(customEditor, pluginEditor, false);
  else
  {
    jassertfalse;
    // \todo open error message box "Open plugin GUI failed."
  }
}

void AudioPluginSlotComponent::openParameterEditor()
{
  ScopedLock lock(*slotToEdit->getMutex());
  if( parameterEditor != nullptr )
  {
    parameterEditor->showInFrontOfAppWindow();
    return; 
  }
  GenericAudioProcessorEditor *pluginEditor = 
    new GenericAudioProcessorEditor(slotToEdit->getPlugin());
  wrapPluginEditorIntoContainerAndShow(parameterEditor, pluginEditor, true);
}

void AudioPluginSlotComponent::wrapPluginEditorIntoContainerAndShow(
  AudioProcessorEditorContainer* &container, AudioProcessorEditor *pluginEditor,                                            
  bool shouldTakeOwnership)
{
  int styleFlags = ComponentPeer::windowHasDropShadow;
  Activatable *containerOwner = dynamic_cast<Activatable*>(getTopLevelComponent());
  container = new AudioProcessorEditorContainer(pluginEditor, shouldTakeOwnership, this, 
                                                containerOwner);
  container->setOpaque(true);
  container->setAlwaysOnTop(true);  
  container->addToDesktop(styleFlags); // after this call, pluginEditor has correct size
  alignWithVisibilityConstraintTo(this, container);
  container->setWantsKeyboardFocus(true);
  container->showInFrontOfAppWindow();
}

void AudioPluginSlotComponent::closeEditors()
{
  closeCustomEditor();
  closeParameterEditor();
}

void AudioPluginSlotComponent::closeCustomEditor()
{
  if( customEditor != nullptr )
    delete customEditor;
  customEditor = nullptr;
}

void AudioPluginSlotComponent::closeParameterEditor()
{
  if( parameterEditor != nullptr )
    delete parameterEditor;
  parameterEditor = nullptr;
}

//=================================================================================================
// class AudioPluginChainComponent:

AudioPluginChainComponent::AudioPluginChainComponent(PluginChain *chainToEdit)
{
  jassert(chainToEdit != nullptr);
  slotHeight  = 14;
  pluginChain = chainToEdit;

  pluginChain->enterLock();
  for(int i = 0; i < pluginChain->pluginSlots.size(); i++)
    pluginChain->pluginSlots[i]->addChangeListener(this);
  tempSlot = new PluginSlot(nullptr, pluginChain->getMutex());
  pluginChain->exitLock();

  tempSlot->addChangeListener(this);
}

AudioPluginChainComponent::~AudioPluginChainComponent()
{
  deleteAllChildren();
  delete tempSlot;

  pluginChain->enterLock();
  for(int i = 0; i < pluginChain->pluginSlots.size(); i++)
    pluginChain->pluginSlots[i]->removeChangeListener(this);
  pluginChain->exitLock();
}

// setup:

void AudioPluginChainComponent::removeLastSlot()
{
  pluginChain->enterLock();
  int numSlots = pluginChain->pluginSlots.size();
  jassert( numSlots == getNumChildComponents()-1 );
  int childIndex = numSlots-1; // numSlots's child is the tempSlotComponent
  Component* child = removeChildComponent(childIndex);
  delete child;
  pluginChain->deleteSlot(numSlots-1);
  pluginChain->exitLock();

  updateSize();
}

// inquiry:
  
bool AudioPluginChainComponent::isLastSlotEmpty()
{
  return isSlotEmpty(pluginChain->pluginSlots.size()-1);
}

bool AudioPluginChainComponent::isSlotEmpty(int index)
{
  ScopedLock lock(*pluginChain->getMutex());
  int numSlots = pluginChain->pluginSlots.size();
  jassert( index < numSlots );
  return pluginChain->pluginSlots[index]->isEmpty();
}

void AudioPluginChainComponent::updateSlotComponents()
{
  deleteAllChildren(); 

  pluginChain->enterLock();
  for(int i = 0; i < pluginChain->pluginSlots.size(); i++)
  {
    AudioPluginSlotComponent *slotComponent = 
      new AudioPluginSlotComponent(pluginChain->pluginSlots[i]);
    slotComponent->setDeletionManager(this);
    addAndMakeVisible(slotComponent);
  }
  pluginChain->exitLock();

  tempSlotComponent = new AudioPluginSlotComponent(tempSlot);
  tempSlotComponent->setRemovable(false);
  addAndMakeVisible(tempSlotComponent);
  updateSize();
}

void AudioPluginChainComponent::changeListenerCallback(ChangeBroadcaster* source)
{
  if( source == tempSlot )
  {
    if( !tempSlot->isEmpty() )
    {
      //pluginChain->enterLock();
      pluginChain->addSlot(tempSlot);
      tempSlot = new PluginSlot(nullptr, pluginChain->getMutex());
      //pluginChain->exitLock();

      tempSlot->addChangeListener(this);
      tempSlotComponent->setRemovable(true);
      tempSlotComponent->setDeletionManager(this);
      tempSlotComponent = new AudioPluginSlotComponent(tempSlot);
      tempSlotComponent->setRemovable(false);
      addAndMakeVisible(tempSlotComponent);
      updateSize();
    }
  }
  else 
  {
    pluginChain->enterLock();
    while( pluginChain->pluginSlots.size() > 0 && isLastSlotEmpty() )
      removeLastSlot();
    pluginChain->exitLock();
  }
}

void AudioPluginChainComponent::handleDeletionRequest(DeletionRequester *object)
{
  AudioPluginSlotComponent *slotComponent = dynamic_cast<AudioPluginSlotComponent*>(object);
  if( slotComponent != nullptr )
  {
    PluginSlot *slot = slotComponent->slotToEdit;
    removeChildComponent(slotComponent);
    delete slotComponent;

    //pluginChain->enterLock();
    pluginChain->deleteSlot(slot);
    //pluginChain->exitLock();

    updateSize();
  }
}

void AudioPluginChainComponent::resized()
{
  int w = getWidth();
  for(int i = 0; i < getNumChildComponents(); i++)
    getChildComponent(i)->setBounds(0, i*(slotHeight-1), w, slotHeight);
}

void AudioPluginChainComponent::updateSize()
{
  setSize(getWidth(), slotHeight*getNumChildComponents());
}
