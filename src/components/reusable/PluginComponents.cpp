#include "PluginComponents.h"


AudioProcessorEditorContainer::AudioProcessorEditorContainer(AudioProcessorEditor *editorToWrap,  
  bool shouldTakeOwnership, DeletionManager *deletor, Activatable *ownerToWatchForActivation) 
: editor(editorToWrap)
, DeletionRequester(deletor)
{
  titleBarHeight = 16;
  ownsEditor     = shouldTakeOwnership;

  addAndMakeVisible(editor);

  visibilityController = ownerToWatchForActivation;
  if( visibilityController != nullptr )
    visibilityController->registerActivationObserver(this);

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

  int buttonSize = titleBarHeight;
  closeButton->setBounds(getWidth()-buttonSize, 0, buttonSize, buttonSize);

  // \todo set up title-bar (close-button, load/save widgets, etc.);
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

//=================================================================================================
// class AudioPluginSlotComponent:

AudioPluginSlotComponent::AudioPluginSlotComponent(PluginSlot *pluginSlotToEdit, 
                                                   PluginChain *pluginChainToUse)
{
  customEditor    = nullptr;
  parameterEditor = nullptr;
  slotToEdit      = pluginSlotToEdit;
  chainToUse      = pluginChainToUse;

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
  if( chainToUse == nullptr )
    delete slotToEdit;  
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
    menu.addItem(1, "Bypass", true, slotToEdit->isBypassed());
  menu.addItem(2, "Load Plugin");
  //menu.addItem(3, "Show Parameters"); // should be under if-clause

  const int result = menu.show();

  if(result == 0)
  {
    // user dismissed the menu without picking anything
  }
  else if(result == 1)
    slotToEdit->setBypass(!slotToEdit->isBypassed());
  else if(result == 2)
    openLoadPluginDialog();
  else if(result == 3)
  {
    // user picked item 2
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
      if( chainToUse != nullptr )
        chainToUse->addSlot(slotToEdit);
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

//=================================================================================================
// class AudioPluginChainComponent:

AudioPluginChainComponent::AudioPluginChainComponent(PluginChain *chainToEdit)
{
  slotHeight  = 14;
  pluginChain = chainToEdit;
  pluginChain->addChangeListener(this);
}

AudioPluginChainComponent::~AudioPluginChainComponent()
{
  pluginChain->removeChangeListener(this);
  deleteAllChildren();
}
/*
void AudioPluginChainComponent::addSlotComponent(AudioPluginSlotComponent *newSlotComponent)
{
  addAndMakeVisible(newSlotComponent);
}
*/
/*
void AudioPluginChainComponent::deleteAllSlotComponents()
{

}
*/
void AudioPluginChainComponent::updateSlotComponents()
{
  deleteAllChildren(); 
    // deletes all slot components - maybe, as an optimization, we may later retain some or all but
    // the code is easier, if we delete them all and recreate them, if necessary

  // create slot components for all slots:
  ScopedLock lock(pluginChain->pluginSlots.getLock());
  for(int i = 0; i < pluginChain->pluginSlots.size(); i++)
    addAndMakeVisible(new AudioPluginSlotComponent(pluginChain->pluginSlots[i], pluginChain));

  // create one additional slot component which is not yet assigned to a slot. there should always 
  // be one empty slot component at the end to be used to plug in another plugins into the chain:
  addAndMakeVisible(new AudioPluginSlotComponent(nullptr, pluginChain));

  setSize(getWidth(), slotHeight*getNumChildComponents());
}

void AudioPluginChainComponent::changeListenerCallback(ChangeBroadcaster* source)
{
  jassert( source == pluginChain);

  //jassertfalse;
    //updateSlotComponents(); //no - that sucks
    // perhaps we should make a pluginChainObserver class and have callbacks 
    // slotAppended, slotInserted, slotRemoved
}

void AudioPluginChainComponent::resized()
{
  int w = getWidth();
  for(int i = 0; i < getNumChildComponents(); i++)
    getChildComponent(i)->setBounds(0, i*(slotHeight-1), w, slotHeight);
}
