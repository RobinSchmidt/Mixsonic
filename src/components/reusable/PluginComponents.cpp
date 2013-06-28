#include "PluginComponents.h"


AudioProcessorEditorContainer::AudioProcessorEditorContainer(AudioProcessorEditor *editorToWrap,  
  bool shouldTakeOwnership) 
: editor(editorToWrap)
{
  titleBarHeight = 16;
  ownsEditor     = shouldTakeOwnership;
  setSize(editor->getWidth(), editor->getHeight() + titleBarHeight);
  addAndMakeVisible(editor);
}

AudioProcessorEditorContainer::~AudioProcessorEditorContainer()
{
  removeAllChildren();
  if( ownsEditor )
    delete editor;
}

// setup:  

void AudioProcessorEditorContainer::showInFrontOfAppWindow()
{
  setVisible(true);
  toFront(true);
}

void AudioProcessorEditorContainer::childBoundsChanged(Component *child)
{
  jassert( child == editor );
  if( editor != nullptr )
    setSize(editor->getWidth(), editor->getHeight() + titleBarHeight);
}

void AudioProcessorEditorContainer::resized()
{
  // \todo set up title-bar (close-button, load/save widgets, etc.);


  if( editor != nullptr )
    editor->setTopLeftPosition(0, titleBarHeight);
}

void AudioProcessorEditorContainer::paint(Graphics &g)
{
  g.setColour(Colours::black); // preliminary
  g.fillRect(0, 0, getWidth(), titleBarHeight);
}

//=================================================================================================
// class AudioPluginSlotComponent:

AudioPluginSlotComponent::AudioPluginSlotComponent(PluginSlot *pluginSlotToEdit, 
                                                   PluginChain *pluginChainToUse)
{
  editor          = nullptr;
  parameterEditor = nullptr;
  //editorIsInFront = false;
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
  delete editor;
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

bool AudioPluginSlotComponent::isEditorVisible()
{
  if( editor == nullptr )
    return false;
  return editor->isVisible();
}

bool AudioPluginSlotComponent::isParameterEditorVisible()
{
  if( parameterEditor == nullptr )
    return false;
  return parameterEditor->isVisible();
}

bool AudioPluginSlotComponent::isAnyEditorVisible()
{
  return isEditorVisible() || isParameterEditorVisible();
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
      //openEditor();
        // actually, the desired behavior is: when the editor is open and in front: hide it, else
        // show it. but it turned out to be hard to find out, if the editor is actually in front
        // of the app-window, so for the time being, we just show it regardless of current state
        // this is some detail for later

      if( !isAnyEditorVisible() )
        openEditor();
      else
        hideEditors();
    }
  }
  else if( e.mods.isRightButtonDown() )
    openPopUpMenu();
}

void AudioPluginSlotComponent::resized()
{
  nameLabel->setBounds(0, 0, getWidth(), getHeight());
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
  menu.addItem(3, "Show Parameters");

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

void AudioPluginSlotComponent::openEditor()
{
  jassert(!isEmpty());

  if( editor != nullptr )
  {
    // editor was already created
    editor->showInFrontOfAppWindow();
    return; 
  }

  if( slotToEdit->plugin->hasEditor() )
  {
    // maybe we should wrap this into a function openCustomEditor:
    AudioProcessorEditor *pluginEditor = slotToEdit->plugin->createEditorIfNeeded();
    if( pluginEditor != nullptr )
      wrapPluginEditorIntoContainerAndShow(editor, pluginEditor, false);
    else
    {
      // \todo open error message box "Open plugin GUI failed."
    }
  }
  else
    openParameterEditor();
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
  container = new AudioProcessorEditorContainer(pluginEditor, shouldTakeOwnership);
  container->setOpaque(true);
  container->setAlwaysOnTop(true);     // not really optimal: it even remains on top when the whole 
                                       // app is brought to background - but i currently don't know
                                       // how to handle that
  container->addToDesktop(styleFlags); // after this call, pluginEditor has correct size
  setupEditorPosition(container);
  container->setWantsKeyboardFocus(true);
  container->showInFrontOfAppWindow();
}

void AudioPluginSlotComponent::hideEditors()
{
  if( editor != nullptr )
    editor->setVisible(false);
  if( parameterEditor != nullptr )
    parameterEditor->setVisible(false);
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

void AudioPluginSlotComponent::setupEditorPosition(AudioProcessorEditorContainer *editor)
{
  // actually, we expect this function to be called only from places where it is guaranteed, that
  // the editor object actually exists:
  jassert( editor != nullptr );
  if( editor == nullptr )
    return; 

  Point<int> topLeft = getScreenPosition();
  topLeft.x += getWidth();
  topLeft = constrainPositionToScreen(topLeft, editor->getWidth(), editor->getHeight());
  editor->setTopLeftPosition(topLeft);
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
