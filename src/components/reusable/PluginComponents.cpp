#include "PluginComponents.h"

AudioProcessorParameterSlider::AudioProcessorParameterSlider(Skin *skinToUse, 
  AudioProcessor *owner, int parameterIndex)
: MixsonicSlider(skinToUse)
{
  this->owner          = owner;
  this->parameterIndex = parameterIndex;

  // we set up the range and related stuff directly without calling setRange, because this would 
  // cause our overriden setValue function to be called which modifies the value inside the plugin:
  currentValue = owner->getParameter(parameterIndex);
  defaultValue = 0.5;
  minValue     = 0.0;
  maxValue     = 1.0;
  interval     = 0.0;
}

void AudioProcessorParameterSlider::setValue(double newValue, const bool sendUpdateMessage, 
  const bool sendMessageSynchronously)
{
  MixsonicSlider::setValue(newValue, sendUpdateMessage, sendMessageSynchronously);
  owner->setParameterNotifyingHost(parameterIndex, (float) currentValue);
}

void AudioProcessorParameterSlider::updateValue()
{
  currentValue = owner->getParameter(parameterIndex);
  repaint();
}

void AudioProcessorParameterSlider::paint(Graphics& g)
{
  g.fillAll(backgroundColour);

  g.setColour(thumbColour);
  float thumbWidth = (float) (getWidth() * valueToProportionOfLength(getValue()));
  g.fillRect(0.f, 0.f, thumbWidth, (float)getHeight());

  g.setColour(skin->foregroundColor);
  g.setFont(  skin->normalFont);

  g.drawText(owner->getParameterName(parameterIndex), 4, 0, getWidth()-4, getHeight(), 
    Justification::centredLeft, false); 
  g.drawText(owner->getParameterText(parameterIndex), 0, 0, getWidth()-4, getHeight(), 
    Justification::centredRight, false); 
  // maybe, we should draw a physical unit here, or is getParameterText supposed to return the 
  // value including the unit?
}

//=================================================================================================
// class AudioProcessorParameterEditor:

AudioProcessorParameterEditor::AudioProcessorParameterEditor(AudioProcessor* owner, 
  SectionSkin *skinToUse)
: AudioProcessorEditor(owner)
, UserInterfaceSection(skinToUse)
{
  int numParams = owner->getNumParameters();

  for(int i = 0; i < numParams; i++)
  {
    AudioProcessorParameterSlider *slider = new AudioProcessorParameterSlider(&skin->widgetSkin, 
      owner, i);
    addAndMakeVisible(slider);
  }

  int width  = 300;
  int height = sliderHeight*numParams + sliderDistance*(numParams-1) + 2*margin;
  setSize(width, height);
}

AudioProcessorParameterEditor::~AudioProcessorParameterEditor()
{
  deleteAllChildren();
}
  
void AudioProcessorParameterEditor::audioProcessorParameterChanged(AudioProcessor* processor, 
  int parameterIndex, float newValue)
{
  ((AudioProcessorParameterSlider*) getChildComponent(parameterIndex))->updateValue();
  // Remark:
  // The implementation is currently kept simple, but may lead to performance issues (crackle), if
  // parameters are automated. If this is the case, look at juce::GenericAudioProcessorEditor and 
  // handle the parameter-update on the GUI in the way like it's done there.
  // JUCE says:
  // IMPORTANT NOTE: this will be called synchronously when a parameter changes, and
  // many audio processors will change their parameter during their audio callback.
  // This means that not only has your handler code got to be completely thread-safe,
  // but it's also got to be VERY fast, and avoid blocking. If you need to handle
  // this event on your message thread, use this callback to trigger an AsyncUpdater
  // or ChangeBroadcaster which you can respond to on the message thread.
}
 
void AudioProcessorParameterEditor::audioProcessorChanged(AudioProcessor* processor)
{
  // don't know what this callback is good for, but if we do something here, the same remark as in
  // audioProcessorParameterChanged applies
}

void AudioProcessorParameterEditor::resized()
{
  int x = margin;
  int y = margin;
  int w = getWidth() - 2*margin;
  int h = sliderHeight;
  for(int i = 0; i < getNumChildComponents(); i++)
  {
    getChildComponent(i)->setBounds(x, y, w, h);
    y += sliderHeight + sliderDistance;
  }
  // \todo - when there are many parameters, we should arrange them in columns
}

void AudioProcessorParameterEditor::paint(Graphics& g)
{
  g.fillAll(  skin->backgroundColor);
  g.setColour(skin->outlineColor);
  g.drawRect(0, 0, getWidth(), getHeight(), 2);
}

//=================================================================================================
// class AudioProcessorEditorContainer:

AudioProcessorEditorContainer::AudioProcessorEditorContainer(SectionSkin *skinToUse, 
  AudioProcessorEditor *editorToWrap,  bool shouldTakeOwnership, DeletionManager *deletor, 
  Activatable *ownerToWatchForActivation) 
: UserInterfaceSection(skinToUse)
, editor(editorToWrap)
, DeletionRequester(deletor)
{
  titleBarHeight = 18;
  ownsEditor     = shouldTakeOwnership;

  addAndMakeVisible(editor);

  visibilityController = ownerToWatchForActivation;
  if( visibilityController != nullptr )
    visibilityController->registerActivationObserver(this);

  nameLabel = new MHeadlineLabel(&skin->labelSkin);
  addAndMakeVisible(nameLabel);
  nameLabel->setText(editor->getAudioProcessor()->getName(), false);
  //nameLabel->addMouseListener(this, false);
  nameLabel->addMouseListener((UserInterfaceSection*)this, false);
  //nameLabel->addMouseListener((MWidget*)this, false);

  closeButton = new MButton(&skin->widgetSkin, "X");
  addAndMakeVisible(closeButton);
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

AudioPluginSlotComponent::AudioPluginSlotComponent(PluginSlot *pluginSlotToEdit, Skin *skinToUse, 
  SectionSkin *skinToUseForEditor)
: MWidget(skinToUse)
{
  jassert(pluginSlotToEdit != nullptr);

  editorSkin = skinToUseForEditor;

  slotToEdit = pluginSlotToEdit;
  slotToEdit->enterLock();
  if( slotToEdit != nullptr )
    slotToEdit->addChangeListener(this);
  slotToEdit->exitLock();

  customEditor    = nullptr;
  parameterEditor = nullptr;
  knownPluginList = nullptr;
  slotIsRemovable = true;

  nameLabel = new MLabel(skin);
  addAndMakeVisible(nameLabel);
  nameLabel->setColour(Label::outlineColourId,    skin->outlineColor);
  nameLabel->setColour(Label::backgroundColourId, skin->backgroundColor);
  nameLabel->setColour(Label::textColourId,       skin->foregroundColor);
  nameLabel->addMouseListener((DescribedMouseListener*)this, false);

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
    // old (draw a cross over the component):
    //g.setColour(Skin::getInstance()->markerColor);
    //g.drawLine(0.f, 0.f, (float) getWidth(), (float) getHeight(), 1.f);
    //g.drawLine(0.f, (float) getHeight(), (float) getWidth(), 0.f, 1.f);
     
    g.setColour(skin->outlineColor);
    drawCrossHatches(g, this, 8.f, 1.f);
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
  loadPluginFromFile(openLoadAudioPluginDialog());
}

void AudioPluginSlotComponent::loadPluginFromFile(const File& pluginFile)
{ 
  closeEditors();
  bool success = slotToEdit->loadPlugin(pluginFile);
  if( success == true )
    openEditor();
  else
    jassertfalse; // \todo show error-message, that the plugin could not be loaded
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
  AudioProcessorParameterEditor *pluginEditor = 
    new AudioProcessorParameterEditor(slotToEdit->getPlugin(), editorSkin);
  wrapPluginEditorIntoContainerAndShow(parameterEditor, pluginEditor, true);
}

void AudioPluginSlotComponent::wrapPluginEditorIntoContainerAndShow(
  AudioProcessorEditorContainer* &container, AudioProcessorEditor *pluginEditor,                                            
  bool shouldTakeOwnership)
{
  int styleFlags = ComponentPeer::windowHasDropShadow;
  Activatable *containerOwner = dynamic_cast<Activatable*>(getTopLevelComponent());
  container = new AudioProcessorEditorContainer(editorSkin, pluginEditor, shouldTakeOwnership, 
    this, containerOwner);
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

AudioPluginChainComponent::AudioPluginChainComponent(PluginChain *chainToEdit, Skin *skinToUse,   
  SectionSkin *skinToUseForEditors)
: MWidget(skinToUse)
{
  jassert(chainToEdit         != nullptr);
  jassert(skinToUseForEditors != nullptr);

  slotHeight  = 14;
  pluginChain = chainToEdit;
  editorSkin  = skinToUseForEditors;

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
      new AudioPluginSlotComponent(pluginChain->pluginSlots[i], skin, editorSkin);
    slotComponent->setDeletionManager(this);
    addAndMakeVisible(slotComponent);
  }
  pluginChain->exitLock();

  tempSlotComponent = new AudioPluginSlotComponent(tempSlot, skin, editorSkin);
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
      tempSlotComponent = new AudioPluginSlotComponent(tempSlot, skin, editorSkin);
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
