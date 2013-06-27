#include "TrackComponent.h"

//-------------------------------------------------------------------------------------------------
// class TrackBodyComponent:

// construction/destruction:

TrackBodyComponent::TrackBodyComponent(Track* newTrackToEdit) 
: GlobalTimeFrameComponent(String("TrackBodyComponent"))
{
  Component::setName(String("TrackBodyComponent"));
  trackToEdit      = newTrackToEdit;
  grabber          = NULL;
  descriptionField = NULL;
  backgroundColour = ::backgroundColor;

  // \todo: if the track initially has some clips on it, we should create the ClipComponents for
  // them now and add them as child components - where is this done on opening a project?
}

TrackBodyComponent::~TrackBodyComponent()
{
  clearGarbage();
  clearAllClipComponents();
  deleteAllChildren();
}

// setup:

void TrackBodyComponent::setSelectedTimeUnit(int newSelectedTimeUnit)
{
  GlobalTimeFrameComponent::setSelectedTimeUnit(newSelectedTimeUnit);
  alignAllClipsToTimeLine(true);
}

void TrackBodyComponent::selectClipsInPixelRegion(bool shouldBeSelected, int x1, int x2, 
                                                  bool temporarily)
{
  audioClipComponents.getLock().enter();
  double t1 = pixelPositionToBeats((double) x1);
  double t2 = pixelPositionToBeats((double) x2);
  for(int c=0; c<audioClipComponents.size(); c++)
  {
    if( audioClipComponents[c]->hasActiveDataBetweenTimeInstants(t1, t2, BEATS, false) )
      audioClipComponents[c]->setSelected(shouldBeSelected, temporarily, false);
  }
  audioClipComponents.getLock().exit();
}

void TrackBodyComponent::setDescriptionField(juce::Label *newDescriptionField)
{
  descriptionField = newDescriptionField;
  audioClipComponents.getLock().enter();
  for(int c=0; c<audioClipComponents.size(); c++)
    audioClipComponents[c]->setDescriptionField(newDescriptionField);
  audioClipComponents.getLock().exit();
}

void TrackBodyComponent::setComponentGrabber(ComponentGrabber *newGrabber)
{
  grabber = newGrabber;
  audioClipComponents.getLock().enter();
  for(int c=0; c<audioClipComponents.size(); c++)
    audioClipComponents[c]->setComponentGrabber(newGrabber);
  audioClipComponents.getLock().exit();
}

void TrackBodyComponent::setBackgroundColour(Colour newColour)
{
  backgroundColour = newColour;
}

void TrackBodyComponent::addAudioClipComponent(AudioClipComponent *componentToAdd)
{
  audioClipComponents.getLock().enter();
  audioClipComponents.add(componentToAdd);
  audioClipComponents.getLock().exit();

  componentToAdd->setDescriptionField(descriptionField);
  componentToAdd->setComponentGrabber(grabber);
  componentToAdd->setMouseMode(mouseMode);
  componentToAdd->addChangeListener(this);
  componentToAdd->setInstantiationManager(this);
  componentToAdd->setDrawingThread(drawingThread);

  alignClipToTimeLine(componentToAdd, true);
  addAndMakeVisible(componentToAdd); 
    // this is now our child-component, so our destructor will delete it via deleteAllChildren
}

void TrackBodyComponent::removeAudioClipComponent(AudioClipComponent *componentToRemove)
{
  componentToRemove->removeChangeListener(this);
  componentToRemove->setInstantiationManager(NULL);
  removeChildComponent(componentToRemove);

  audioClipComponents.getLock().enter();
  audioClipComponents.removeValue(componentToRemove);
  audioClipComponents.getLock().exit();
}

// inquiry:

bool TrackBodyComponent::isSomeClipAtX(int x) const
{
  audioClipComponents.getLock().enter();
  for(int i=audioClipComponents.size()-1; i>=0; i--)
  {
    if(  audioClipComponents[i]->getX()     <= x
      && audioClipComponents[i]->getRight() >  x  ) //  mmm... > works better than >= ...why?
    {
      audioClipComponents.getLock().exit();
      return true;
    }
  }
  audioClipComponents.getLock().exit();
  return false;
}

int TrackBodyComponent::findComponentForClip(Clip *clipToFindComponentFor)
{
  audioClipComponents.getLock().enter();
  for(int c=0; c<audioClipComponents.size(); c++)
  {
    //audioClipComponents[c]->lockUnderlyingClip();
    audioClipComponents[c]->lockClipAndBufferPointers();
    if( audioClipComponents[c]->getUnderlyingClip() == clipToFindComponentFor )
    {
      //audioClipComponents[c]->unlockUnderlyingClip();
      audioClipComponents[c]->unlockClipAndBufferPointers();
      audioClipComponents.getLock().exit();    
      return c;
    }
    audioClipComponents[c]->unlockClipAndBufferPointers();
    //audioClipComponents[c]->unlockUnderlyingClip();
  }
  audioClipComponents.getLock().exit();
  return -1;
}

// user interface related callbacks:

void TrackBodyComponent::clearTemporarySelectionStates()
{
  audioClipComponents.getLock().enter();
  for(int c=0; c<audioClipComponents.size(); c++)
    audioClipComponents[c]->setTemporarilySelected(false, false);
  audioClipComponents.getLock().exit();
}

void TrackBodyComponent::selectAllClips(bool shouldBeSelected)
{
  audioClipComponents.getLock().enter();
  for(int c=0; c<audioClipComponents.size(); c++)
    audioClipComponents[c]->setSelected(shouldBeSelected, false, false);
  audioClipComponents.getLock().exit();
}

void TrackBodyComponent::refreshClipComponents()
{
  audioClipComponents.getLock().enter();
  trackToEdit->audioClips.getLock().enter();

  clearAllClipComponents();

  // for each Clip on the underlying Track, create a corresponding ClipComponent and add it as 
  // child-component to this TrackBodyComponent:
  AudioClipComponent *newClipComponent;
  for(int c=0; c<trackToEdit->audioClips.size(); c++)
  {
    newClipComponent = new AudioClipComponent(trackToEdit->audioClips[c]);
    addAudioClipComponent(newClipComponent);
  }

  trackToEdit->audioClips.getLock().exit();
  audioClipComponents.getLock().exit();
}

void TrackBodyComponent::updateClipComponents()
{
  refreshClipComponents(); 
  // \todo: create new components only for clips which do not already have a Component instead of 
  // just deleting and re-creating all
}

bool TrackBodyComponent::dropComponent(Component* droppedComponent, int x)
{
  // check, whether the droppedComponent is of subclass AudioClipComponent via dynamic_cast - if
  // so, we put it in our array of AudioClipComponents and will from now on treat it as our 
  // child-component:
  AudioClipComponent *newClipComponent = dynamic_cast<AudioClipComponent*>(droppedComponent);

  if( newClipComponent != NULL )
  {
    // store the edited clip in the edited track:
    //newClipComponent->lockUnderlyingClip();
    newClipComponent->lockClipAndBufferPointers();
    trackToEdit->addClip(newClipComponent->getUnderlyingAudioClip());
    double positionOffset 
      = newClipComponent->getUnderlyingAudioClip()->getActiveRegionStart(BEATS, true);
    newClipComponent->unlockClipAndBufferPointers();
    //newClipComponent->unlockUnderlyingClip();

    // setup the corresponding ClipComponent and add it to the array:
    double posInBeats = jmax(pixelPositionToBeats(x), 0.0); // do not allow positioning before zero
    newClipComponent->setClipPosition(posInBeats-positionOffset, BEATS);

    addAudioClipComponent(newClipComponent);
    return true;
  }

  // wrong kind of Component dropped - we are not interested in taking it over and return false to 
  // the caller:
  jassertfalse; // actually, the caller should already make sure that only the right kind of 
                // component is dropped
  return false;
}

Array<AudioClipComponent*> TrackBodyComponent::grabSelectedClips(bool grabCopy)
{
  Array<AudioClipComponent*> returnedArray;
  AudioClipComponent* clipComponentToAdd;
  audioClipComponents.getLock().enter();
  for(int c=0; c<audioClipComponents.size(); c++)
  {
    if( audioClipComponents[c]->isSelected() )
    {
      if( grabCopy == true )
      {
        // use the copy-constructor to create a copy and add it to the array:
        clipComponentToAdd = new AudioClipComponent( *audioClipComponents[c] );
        alignClipToTimeLine(clipComponentToAdd, false); 
          // \todo: check why passing true does not work
        returnedArray.add(clipComponentToAdd);
      }
      else
      {
        // add the original component to the array and remove it from the track:
        clipComponentToAdd = audioClipComponents[c];
        alignClipToTimeLine(clipComponentToAdd, false);
          // \todo: check why passing true does not work

        removeAudioClipComponent(clipComponentToAdd);
        trackToEdit->removeClip(c, false);
        returnedArray.add(clipComponentToAdd);
        c--;    // because we have just shrunken audioClipComponents
      }
    }
  }
  audioClipComponents.getLock().exit();
  return returnedArray;
}

void TrackBodyComponent::setTimeRangeInSeconds(double newMinTime, double newMaxTime)
{
  GlobalTimeFrameComponent::setTimeRangeInSeconds(newMinTime, newMaxTime);
  alignAllClipsToTimeLine(true);
}

void TrackBodyComponent::changeListenerCallback(ChangeBroadcaster *objectThatHasChanged)
{
  audioClipComponents.getLock().enter();
  for(int c=0; c<audioClipComponents.size(); c++)
  {
    if( objectThatHasChanged == audioClipComponents[c] )
      alignClipToTimeLine( audioClipComponents[c], true );
  }
  audioClipComponents.getLock().exit();
}

void TrackBodyComponent::handleDeletionRequest(Clip *clip)
{
  audioClipComponents.getLock().enter();
  int componentIndex = findComponentForClip(clip);
  if( componentIndex == -1 )
  {
    audioClipComponents.getLock().exit();
    return;
  }

  AudioClipComponent* clipComponentToRemove = audioClipComponents[componentIndex];
  removeAudioClipComponent(clipComponentToRemove);
  trackToEdit->removeClip(componentIndex, false);
      // this could be done in removeAudioClipComponent() by passing a flag 

  // we can't directly delete the clip or its component because the call to this function may 
  // originate inside the clip/component which is going to be deleted itself - that would mean that
  // an object indirectly deletes itself from within its member function which causes severe bugs. 
  // instead, we move the clip/component into some special garbage area which is cleared on 
  // destruction:
  garbageComponents.getLock().enter();
  clipComponentToRemove->assignAudioFileBuffer(NULL);
  garbageComponents.add(clipComponentToRemove);
  garbageComponents.getLock().exit();

  garbageClips.getLock().enter();
  garbageClips.add(clip);
  garbageClips.getLock().exit();

  audioClipComponents.getLock().exit();
}

void TrackBodyComponent::handleSplitRequest(Clip* clip, double timeInstantAtWhichToSplit, int timeUnit, 
                                        bool inLocalTime)
{
  audioClipComponents.getLock().enter();
  int componentIndex = findComponentForClip(clip);
  if( componentIndex == -1 )
  {
    audioClipComponents.getLock().exit();
    return;
  }

  // create a copy of the ClipComonent in question (this also creates a copy of the underlying 
  // clip):
  AudioClipComponent* oldClipComponent = audioClipComponents[componentIndex];
  AudioClipComponent* newClipComponent = new AudioClipComponent(*oldClipComponent);

  // set up new active regions and fade-settings for both clips:
  oldClipComponent->setActiveRegionEnd(timeInstantAtWhichToSplit, timeUnit, inLocalTime);
  oldClipComponent->setFadeOutInSeconds(0.01);
  newClipComponent->setActiveRegionStart(timeInstantAtWhichToSplit, timeUnit, inLocalTime);
  newClipComponent->setFadeInInSeconds(0.01);

  alignClipToTimeLine(oldClipComponent, true);
  addAudioClipComponent(newClipComponent);

  //newClipComponent->lockUnderlyingClip();  
  newClipComponent->lockClipAndBufferPointers();
  trackToEdit->addClip(newClipComponent->getUnderlyingAudioClip());
  newClipComponent->unlockClipAndBufferPointers();
  //newClipComponent->unlockUnderlyingClip(); 
   // \todo: do this stuff in addAudioClipComponent - pass a boolean flag to this function to 
   // indicate wheteher or not thsi should be done

  audioClipComponents.getLock().exit();
}

void TrackBodyComponent::resized()
{
  Component::resized();
  alignAllClipsToTimeLine(true);
}

void TrackBodyComponent::paint(Graphics &g)
{
  g.fillAll(backgroundColour);
  g.setColour(outlineColor);
  g.drawRect(0, 0, getWidth(), getHeight());
}

// internal functions:

void TrackBodyComponent::clearAllClipComponents()
{
  audioClipComponents.getLock().enter();
  while( audioClipComponents.size() > 0 )
  {
    audioClipComponents[0]->removeChangeListener(this);
    audioClipComponents[0]->setInstantiationManager(NULL);
    removeChildComponent(audioClipComponents[0]);
    delete audioClipComponents[0];               // deletes the object itself
    audioClipComponents.remove(0);               // removes the pointer from the array
  }
  audioClipComponents.getLock().exit();
}

void TrackBodyComponent::clearGarbage()
{
  garbageComponents.getLock().enter();
  garbageComponents.clear(true);
  garbageComponents.getLock().exit();

  garbageClips.getLock().enter();
  garbageClips.clear(true);
  garbageClips.getLock().exit();
}

void TrackBodyComponent::setDrawingThread(TimeSliceThread* newDrawingThread)
{
  drawingThread = newDrawingThread;
  audioClipComponents.getLock().enter();
  for(int c=0; c<audioClipComponents.size(); c++)
    audioClipComponents[c]->setDrawingThread(newDrawingThread);
  audioClipComponents.getLock().exit();
}

void TrackBodyComponent::alignAllClipsToTimeLine(bool restrictBoundsToVisibleRange)
{
  audioClipComponents.getLock().enter();
  for(int c=0; c<audioClipComponents.size(); c++)
    alignClipToTimeLine(c, restrictBoundsToVisibleRange);
  audioClipComponents.getLock().exit();
}

void TrackBodyComponent::alignClipToTimeLine(int clipIndex, bool restrictBoundsToVisibleRange)
{
  audioClipComponents.getLock().enter();
  if( clipIndex >= 0 && clipIndex < audioClipComponents.size() )
    alignClipToTimeLine(audioClipComponents[clipIndex], restrictBoundsToVisibleRange);
  audioClipComponents.getLock().exit();
}

void TrackBodyComponent::alignClipToTimeLine(AudioClipComponent *clipToAlign, 
                                         bool restrictBoundsToVisibleRange)
{
  if( clipToAlign == NULL )
    return;

  double tMin = getMinTimeInBeats();
  double tMax = getMaxTimeInBeats();
  if( !clipToAlign->hasActiveDataBetweenTimeInstants(tMin, tMax, BEATS, false) )
    clipToAlign->setVisible(false);
  else
  {
    clipToAlign->setVisible(true);

    // obtain position, start and end with respect to the track's time axis:
    double position          = clipToAlign->getClipPosition(BEATS);
    double regionStartLocal  = clipToAlign->getActiveRegionStart(BEATS, true);
    double regionEndLocal    = clipToAlign->getActiveRegionEnd(  BEATS, true); 
    double regionStartGlobal = clipToAlign->getActiveRegionStart(BEATS, false);
    double regionEndGlobal   = clipToAlign->getActiveRegionEnd(  BEATS, false); 

    // set up the bounds of the clip:
    double xL = beatsToPixelPosition(regionStartGlobal);
    double xR = beatsToPixelPosition(regionEndGlobal);
    //int xR = xL + (int) beatsToPixelPosition(endInTrack-startInTrack);

    if( restrictBoundsToVisibleRange == true )
    {
      xL = jlimit(0.0, (double) getWidth(), xL);
      xR = jlimit(0.0, (double) getWidth(), xR);
    }
    double w = jmax((double)MIN_CLIPWIDTH, xR-xL);
    clipToAlign->setBounds((int) round(xL), 0, (int) round(w), getHeight());  

    // setup the visible range inside the clip:
    double minVisible = jmax(regionStartLocal, tMin-position);
    double maxVisible = jmin(regionEndLocal,   tMax-position);
    if( restrictBoundsToVisibleRange == true )
    {
      clipToAlign->setTimeRangeToRender(   beatsToSeconds(minVisible), beatsToSeconds(maxVisible));
      clipToAlign->setTimeRangeInComponent(beatsToSeconds(minVisible), beatsToSeconds(maxVisible));
    }
    else
    {
      clipToAlign->setTimeRangeToRender(   beatsToSeconds(regionStartLocal), beatsToSeconds(regionEndLocal));
      clipToAlign->setTimeRangeInComponent(beatsToSeconds(regionStartLocal), beatsToSeconds(regionEndLocal));
    }
  }
}








AudioPluginSlotComponent::AudioPluginSlotComponent(PluginSlot *pluginSlotToEdit, 
                                                   PluginChain *pluginChainToUse)
{
  slotToEdit = pluginSlotToEdit;
  chainToUse = pluginChainToUse;

  addAndMakeVisible( nameLabel = new RLabel() );
  nameLabel->setColour(Label::outlineColourId, outlineColor);
  nameLabel->setColour(Label::backgroundColourId, backgroundColor);
  nameLabel->setColour(Label::textColourId, textColor);
  nameLabel->addMouseListener(this, false);

  addAndMakeVisible( onOffButton = new RButton("M") );
  onOffButton->setDescription("Switches plugin on/off");
  onOffButton->setClickingTogglesState(true);
  //onOffButton->addListener(this); 

  if( slotToEdit != nullptr )
    slotToEdit->addChangeListener(this);

  updateLabelText();
}

AudioPluginSlotComponent::~AudioPluginSlotComponent()
{
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
      openEditor();
  }
  else if( e.mods.isRightButtonDown() )
    openPopUpMenu();
}

void AudioPluginSlotComponent::resized()
{
  nameLabel->setBounds(0, 0, getWidth(), getHeight());

  //int h = getHeight();
  //nameLabel->setBounds(0, 0, getWidth()-h, h);
  //onOffButton->setBounds(nameLabel->getRight(), 0, h, h);
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

  if( slotToEdit->plugin->hasEditor() )
  {
    AudioProcessorEditor *editor = slotToEdit->plugin->createEditorIfNeeded();
    if( editor != nullptr )
    {
      editor->setOpaque(true);
      editor->setVisible(true);
      editor->setTopLeftPosition(100, 100);
      //int styleFlags = ComponentPeer::windowIsTemporary | ComponentPeer::windowHasCloseButton 
      //  | ComponentPeer::windowHasDropShadow;
      int styleFlags = ComponentPeer::windowHasTitleBar | ComponentPeer::windowHasCloseButton;
      editor->addToDesktop(styleFlags);
    }
    else
    {
      // \todo open error message box "Open plugin GUI failed."
    }
  }
  else
  {
    // openGenericParameterEditor
    int dummy = 0;
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
}

//=================================================================================================

AudioPluginChainComponent::AudioPluginChainComponent(PluginChain *chainToEdit)
{
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
}

void AudioPluginChainComponent::changeListenerCallback(ChangeBroadcaster* source)
{
  jassert( source == pluginChain);

  jassertfalse; // we have something to do here
}

void AudioPluginChainComponent::resized()
{
  int h = 14;
  int w = getWidth();
  for(int i = 0; i < getNumChildComponents(); i++)
    getChildComponent(i)->setBounds(0, i*(h-1), w, h);
}

//=================================================================================================
// class TrackControlComponent:

// construction/destruction:

MixsonicTrackControlComponent::MixsonicTrackControlComponent(Track* newTrackToEdit) 
: Component(String("MixsonicTrackControlComponent"))
{
  trackToEdit = newTrackToEdit;

  addAndMakeVisible( nameLabel = new RLabel() );
  nameLabel->setEditable(false, true);
  nameLabel->addListener(this);
  if( trackToEdit != nullptr )
    nameLabel->setText(trackToEdit->getName(), false);

  addAndMakeVisible( muteButton = new RButton("M") );
  muteButton->setDescription(muteStr);
  muteButton->setClickingTogglesState(true);
  muteButton->addListener(this);

  addAndMakeVisible( soloButton = new RButton("S") );
  soloButton->setDescription(soloStr);
  soloButton->setClickingTogglesState(true);
  soloButton->addListener(this);

  addAndMakeVisible( levelSlider = new MixsonicSlider(levelStr) );
  levelSlider->setDescription(levelSliderStr);
  levelSlider->setRange(-30.0, 6.0, 0.1, 0.0, true);
  levelSlider->setVertical(true);
  levelSlider->addListener(this);

  addAndMakeVisible( panSlider = new MixsonicSlider(panStr) );
  panSlider->setDescription(panSliderStr);
  panSlider->setRange(-1.0, 1.0, 0.01, 0.0, true);
  panSlider->setThumbColour(Colours::transparentBlack);
  panSlider->addListener(this);

  pluginChainComponent = new AudioPluginChainComponent(&trackToEdit->pluginChain);
  addAndMakeVisible(pluginChainComponent);
  //pluginChainComponent->addSlotComponent(new AudioPluginSlotComponent);
  //pluginChainComponent->addSlotComponent(new AudioPluginSlotComponent);
  //pluginChainComponent->addSlotComponent(new AudioPluginSlotComponent);

  trackToEdit->registerObserver(this);

  updateWidgetsAccordingToState();
}

MixsonicTrackControlComponent::~MixsonicTrackControlComponent()
{
  deleteAllChildren();
}

//-------------------------------------------------------------------------------------------------
// setup:

void MixsonicTrackControlComponent::setDescriptionField(Label* newDescriptionField)
{
  soloButton->setDescriptionField(newDescriptionField);
  muteButton->setDescriptionField(newDescriptionField);
  levelSlider->setDescriptionField(newDescriptionField);
  panSlider->setDescriptionField(newDescriptionField);
}

void MixsonicTrackControlComponent::updateWidgetsAccordingToState()
{
  if( trackToEdit != NULL )
  {
    nameLabel->setText(        trackToEdit->getName(),        false);
    levelSlider->setValue(     trackToEdit->getVolumeLevel(), false, false);
    panSlider->setValue(       trackToEdit->getPan(),         false, false);
    soloButton->setToggleState(trackToEdit->isSolo(),         false);
    muteButton->setToggleState(trackToEdit->isMuted(),        false);
    pluginChainComponent->updateSlotComponents();;
  }
}

//-------------------------------------------------------------------------------------------------
// callbacks:

void MixsonicTrackControlComponent::labelTextChanged(Label *labelThatHasChanged)
{
  if( trackToEdit == NULL )
    return;
  if( labelThatHasChanged == nameLabel )
    trackToEdit->setName(nameLabel->getText());
}

void MixsonicTrackControlComponent::buttonClicked(Button *buttonThatWasClicked)
{
  if( trackToEdit == NULL )
    return;

  if( buttonThatWasClicked == muteButton )
  {
    trackToEdit->setMute( muteButton->getToggleState() );
    repaint();
  }
  else if( buttonThatWasClicked == soloButton )
    trackToEdit->setSolo( soloButton->getToggleState() );

  sendChangeMessage();
}

void MixsonicTrackControlComponent::mSliderValueChanged(MixsonicSlider *mSliderThatHasChanged)
{
  if( trackToEdit == NULL )
    return;

  if( mSliderThatHasChanged == levelSlider )
    trackToEdit->setVolumeLevel( levelSlider->getValue() );
  else if( mSliderThatHasChanged == panSlider )
    trackToEdit->setPan( panSlider->getValue() );

  sendChangeMessage(); 
}

//-------------------------------------------------------------------------------------------------
// appearance stuff:

void MixsonicTrackControlComponent::resized()
{
  int sliderSize = 12;
  int buttonSize = 16;
  int margin = 4;

  int x = margin;
  int y = margin;
  int w = sliderSize;
  int h = getHeight() - 2*margin;
  levelSlider->setBounds(x, y, w, h); 

  x = levelSlider->getRight() + margin;
  w = getWidth() - x - margin;
  nameLabel->setBounds(x, y, w, buttonSize);

  y = nameLabel->getBottom() + margin;
  soloButton->setBounds(x, y, buttonSize, buttonSize);
  x += buttonSize;
  muteButton->setBounds(x, y, buttonSize, buttonSize);

  x = levelSlider->getRight() + margin;
  y = soloButton->getBottom() + margin;
  w = 2*buttonSize;
  h = sliderSize;
  panSlider->setBounds(x, y, w, h);

  x = muteButton->getRight() + margin;
  y = nameLabel->getBottom() + margin;
  w = getWidth()  - x - margin;
  h = getHeight() - y - margin;
  pluginChainComponent->setBounds(x, y, w, h);

  // remark: this arrangement of widgets suggests that level- and pan slider are applied before the 
  // plugins, i.e. the plugins are in post-fader configuration. it should probably be the other way 
  // around, or better, selectable by the user with pre-fader configuration as default. the widget
  // arrangement adapt according to the setting
}

void MixsonicTrackControlComponent::paint(Graphics &g)
{  
  g.setColour(outlineColor);
  g.drawRect(0, 0, getWidth(), getHeight());
  // \todo: gray out all track components which currently are not audible - but maybe do this for 
  // the actual track components - not the control-components
}

void MixsonicTrackControlComponent::trackSoloStateChanged(Track *track, bool isSolo)
{
  soloButton->setToggleState(isSolo, false);
}

//=================================================================================================
// class TrackComponent:
  
TrackComponent::TrackComponent(Track* newTrackToEdit)
{
  bodyComponent = new TrackBodyComponent(newTrackToEdit);
  addAndMakeVisible(bodyComponent);


  outputComponent = new MixsonicTrackControlComponent(newTrackToEdit);
  addAndMakeVisible(outputComponent);
}

TrackComponent::~TrackComponent()
{
  deleteAllChildren();
}

bool TrackComponent::canLassoSelectBeginHere(int x) const
{
  if( x < bodyComponent->getX() || x > bodyComponent->getRight() )
    return false;
  else
    return !bodyComponent->isSomeClipAtX(x-bodyComponent->getX());
}

void TrackComponent::resized()
{
  int outputComponentWidth = 160;

  int x = 0;
  int y = 0;
  int w = outputComponentWidth;
  int h = getHeight();
  outputComponent->setBounds(x, y, w, h);

  x = outputComponent->getRight();
  w = getWidth() - x;
  bodyComponent->setBounds(x, y, w, h);
}

bool TrackComponent::dropComponent(Component* droppedComponent, int x)
{
  return bodyComponent->dropComponent(droppedComponent, x-bodyComponent->getX());
}

void TrackComponent::selectClipsInPixelRegion(bool shouldBeSelected, int x1, int x2, 
                                              bool temporarily)
{
  x1 -= bodyComponent->getX();
  x2 -= bodyComponent->getX();
  bodyComponent->selectClipsInPixelRegion(shouldBeSelected, x1, x2, temporarily);
}
