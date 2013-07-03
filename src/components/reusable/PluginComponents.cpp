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

  customEditor    = nullptr;
  parameterEditor = nullptr;
  slotToEdit      = pluginSlotToEdit;
  slotIsRemovable = true;

  addAndMakeVisible( nameLabel = new RLabel() );
  nameLabel->setColour(Label::outlineColourId, outlineColor);
  nameLabel->setColour(Label::backgroundColourId, backgroundColor);
  nameLabel->setColour(Label::textColourId, textColor);
  nameLabel->addMouseListener(this, false);

  if( slotToEdit != nullptr )
    slotToEdit->addChangeListener(this);

  updateLabelText();
}

AudioPluginSlotComponent::~AudioPluginSlotComponent()
{
  delete customEditor;
  delete parameterEditor;
  if( slotToEdit != nullptr )
    slotToEdit->removeChangeListener(this); 
  deleteAllChildren();
}

// inquiry:

bool AudioPluginSlotComponent::isEmpty()
{
  return slotToEdit == nullptr || slotToEdit->plugin == nullptr;
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
  if( isEmpty() )
    nameLabel->setText("insert plugin", false);
  else
    nameLabel->setText(slotToEdit->plugin->getName(), false);
}

// internal:

void AudioPluginSlotComponent::openPopUpMenu()
{
  PopupMenu menu;

  if( !isEmpty() )
  {
    menu.addItem(1, "Bypass", true, slotToEdit->isBypassed());
    menu.addItem(2, "Show Parameters"); 
    menu.addItem(3, "Open GUI", slotToEdit->plugin->hasEditor()); 
    menu.addItem(4, "Remove Plugin"); 
    menu.addSeparator();
  }
  if( slotIsRemovable ) 
    menu.addItem(5, "Remove Slot"); 
  menu.addItem(6, "Load Plugin");

  const int result = menu.show();

  switch( result )
  {
  case 1: slotToEdit->setBypass(!slotToEdit->isBypassed()); break;
  case 2: openParameterEditor();                            break;
  case 3: openCustomEditor();                               break;
  case 4: slotToEdit->setPlugin(nullptr, true);             break;
  case 5: requestDeletion();                                break;
  case 6: openLoadPluginDialog();                           break;
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
  if( slotToEdit == nullptr )
  {
    AudioPluginInstance *plugin = getVSTPluginInstanceFromFile(pluginFile);
    if( plugin != nullptr )
    {
      slotToEdit = new PluginSlot(plugin);
      slotToEdit->addChangeListener(this);
      updateLabelText();
    }
  }
  else
  {
    slotToEdit->loadPlugin(pluginFile);
      // update of the label-text will be triggered by a changeListenerCallback in this branch
  }
  openEditor();
}

void AudioPluginSlotComponent::openEditor()
{
  jassert(!isEmpty());
  if( slotToEdit->plugin->hasEditor() )
    openCustomEditor();
  else
    openParameterEditor();
}

void AudioPluginSlotComponent::openCustomEditor()
{
  if( customEditor != nullptr )
  {
    customEditor->showInFrontOfAppWindow();
    return; 
  }
  AudioProcessorEditor *pluginEditor = slotToEdit->plugin->createEditorIfNeeded();
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
  if( parameterEditor != nullptr )
  {
    parameterEditor->showInFrontOfAppWindow();
    return; 
  }
  GenericAudioProcessorEditor *pluginEditor = new GenericAudioProcessorEditor(slotToEdit->plugin);
  wrapPluginEditorIntoContainerAndShow(parameterEditor, pluginEditor, true);
}

void AudioPluginSlotComponent::wrapPluginEditorIntoContainerAndShow(AudioProcessorEditorContainer* &container, 
                                                                    AudioProcessorEditor *pluginEditor, 
                                                                    bool shouldTakeOwnership)
{
  int styleFlags = ComponentPeer::windowHasDropShadow;
  Activatable *containerOwner = dynamic_cast<Activatable*>(getTopLevelComponent());
  container = new AudioProcessorEditorContainer(pluginEditor, shouldTakeOwnership, this, containerOwner);
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
/*
void AudioPluginSlotComponent::removePlugin()
{

}
*/

//=================================================================================================
// class AudioPluginChainComponent:

AudioPluginChainComponent::AudioPluginChainComponent(PluginChain *chainToEdit)
{
  slotHeight  = 14;
  pluginChain = chainToEdit;
  for(int i = 0; i < pluginChain->pluginSlots.size(); i++)
    pluginChain->pluginSlots[i]->addChangeListener(this);
  tempSlot = new PluginSlot(nullptr);
  tempSlot->addChangeListener(this);
}

AudioPluginChainComponent::~AudioPluginChainComponent()
{
  for(int i = 0; i < pluginChain->pluginSlots.size(); i++)
    pluginChain->pluginSlots[i]->removeChangeListener(this);
  delete tempSlot;
  deleteAllChildren();
}

// setup:
/*
void AudioPluginChainComponent::appendEmptySlot()
{
  ScopedLock lock(pluginChain->pluginSlots.getLock());
  PluginSlot *emptySlot = new PluginSlot(nullptr);
  emptySlot->addChangeListener(this);
  addAndMakeVisible(new AudioPluginSlotComponent(emptySlot));
}
*/

void AudioPluginChainComponent::removeLastSlot()
{
  ScopedLock lock(pluginChain->pluginSlots.getLock());
  int numSlots = pluginChain->pluginSlots.size();
  jassert( numSlots == getNumChildComponents() );
  Component* child = removeChildComponent(numSlots);
  delete child;
  //pluginChain->removeSlot(numSlots, true);
  pluginChain->deleteSlot(numSlots-1);
  updateSize();
}

// inquiry:
  
bool AudioPluginChainComponent::isLastSlotEmpty()
{
  ScopedLock lock(pluginChain->pluginSlots.getLock());
  int numSlots = pluginChain->pluginSlots.size();
  return isSlotEmpty(numSlots-1);
}

bool AudioPluginChainComponent::isSlotEmpty(int index)
{
  ScopedLock lock(pluginChain->pluginSlots.getLock());
  int numSlots = pluginChain->pluginSlots.size();
  jassert( index < numSlots );
  return pluginChain->pluginSlots[index]->isEmpty();
}

void AudioPluginChainComponent::updateSlotComponents()
{
  deleteAllChildren(); 
  ScopedLock lock(pluginChain->pluginSlots.getLock());
  for(int i = 0; i < pluginChain->pluginSlots.size(); i++)
  {
    AudioPluginSlotComponent *slotComponent = 
      new AudioPluginSlotComponent(pluginChain->pluginSlots[i]);
    slotComponent->setDeletionManager(this);
    addAndMakeVisible(slotComponent);
  }
  tempSlotComponent = new AudioPluginSlotComponent(tempSlot);
  tempSlotComponent->setRemovable(false);
  addAndMakeVisible(tempSlotComponent);
  updateSize();
}

void AudioPluginChainComponent::changeListenerCallback(ChangeBroadcaster* source)
{
  ScopedLock lock(pluginChain->pluginSlots.getLock());
  if( source == tempSlot )
  {
    if( !tempSlot->isEmpty() )
    {
      pluginChain->addSlot(tempSlot);
      tempSlot = new PluginSlot(nullptr);
      tempSlot->addChangeListener(this);

      tempSlotComponent->setRemovable(true);
      tempSlotComponent->setDeletionManager(this);

      tempSlotComponent = new AudioPluginSlotComponent(tempSlot);
      tempSlotComponent->setRemovable(false);
      addAndMakeVisible(tempSlotComponent);

      updateSize();
    }
  }
  else if( isLastSlotEmpty() )
    removeLastSlot();
}

void AudioPluginChainComponent::handleDeletionRequest(DeletionRequester *object)
{
  ScopedLock lock(pluginChain->pluginSlots.getLock());

  // \todo - find out the correspondi

  AudioPluginSlotComponent *slotComponent = dynamic_cast<AudioPluginSlotComponent*>(object);
  if( slotComponent != nullptr )
  {
    PluginSlot *slot = slotComponent->slotToEdit;
    removeChildComponent(slotComponent);
    delete slotComponent;
    pluginChain->deleteSlot(slot);
    resized();
  }

  int dummy = 0;
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
