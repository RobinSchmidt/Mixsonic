//#include "MixsonicArrangementPanel.h"
#include "MixsonicArrangementEditor.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction and static member initialization:

const double MixsonicArrangementPanel::autoScrollThreshold = 0.8;

MixsonicArrangementPanel::MixsonicArrangementPanel(SectionSkin *skinToUse, 
  Arrangement* newArrangementToEdit, SamplePool* samplePoolToUse) 
: Panel(String("MixsonicArrangementPanel")), SamplePoolClient(samplePoolToUse)
, UserInterfaceSection(skinToUse)
{
  trackHeight  = TrackComponent::getMinimumHeight(); // maybe use a function getOptimumHeight()
  trackOffsetY = 0.0;

  jassert(newArrangementToEdit != NULL);
  arrangementToEdit = newArrangementToEdit;

  // register ourselves as SelectionListener for all the clips and as watcher for changes in the 
  // SamplePool:
  arrangementToEdit->registerSelectionListener(this);
  samplePool->addClient(this);

  // initialize the pointer to field where the descriptions appear:
  descriptionField = NULL;

  // intialize the grabber which grabs the clips:
  grabber = NULL;

  // create the timeline:
  timeLine = new TimeLineComponent(&skin->widgetSkin); // maybe pass the labelSkin instead
  addAndMakeVisible(timeLine);
  timeLine->addChangeListener(this);
  timeLine->setAlwaysOnTop(true); 
    // avoids getting track-components in front of it when scrolling down

  // create the component that represents the time cursor:
  addAndMakeVisible( timeCursor = new RectangleComponent() );
  timeCursor->setFillColor(   skin->foregroundColor);
  timeCursor->setOutlineColor(skin->foregroundColor);
  timeCursor->setAlwaysOnTop(true);

  // create all the track-components and add them as child components here:
  //mouseMode = ClipComponent::MOVE_CLIPS; // deprecated
  createTrackComponents();

  // some initilizations:
  setMaximumRangeX(0.0, beatsToSeconds(arrangementToEdit->getMaxTimeInBeats(), 
    arrangementToEdit->getGlobalTempoInBpm()));
  setCurrentRangeX(0.0, 12.0);
  setMaximumRangeY(0.0, 1.0);
  setCurrentRangeY(0.0, 1.0);
  setMinimumWidth(0.1); // keep at least 0.1 seconds are always visible
  lassoRectangle = NULL;

  //setSize(100, 100); // we need some nonzero initial size, otherwise we hit a breakpoint in 
  //                   // PanelRange
}

MixsonicArrangementPanel::~MixsonicArrangementPanel()
{
  deleteAllChildren();
}

//-------------------------------------------------------------------------------------------------
// setup:

void MixsonicArrangementPanel::initializeArrangement()
{
  trackComponents.getLock().enter();

  // clear the old, obsolete array of TrackComponents:
  clearTrackComponents();
   
  // initialize the actual underlying Arrangement:
  arrangementToEdit->initializeArrangement();
    
  // create the new TrackComponents according to the new Arrangement:
  createTrackComponents();

  // let each TrackComponent create its child compoents for the clips:
  for(int t=0; t<trackComponents.size(); t++)
    trackComponents[t]->bodyComponent->refreshClipComponents();

  // update the time cursor:
  adjustTimeCursorPosition();

  // update the visible range:
  double length = arrangementToEdit->getMaxTimeInBeats();
  double bpm    = arrangementToEdit->getGlobalTempoInBpm();
  timeLine->setTimeRangeInBeats(0.0, length);
  if( arrangementToEdit->selectedTimeUnit == TimeUnitConverter::BEATS )
  {
    setMaximumRangeX(0.0, length);
    setCurrentRangeX(0.0, length);
  }
  else // SECONDS
  {
    setMaximumRangeX(0.0, beatsToSeconds(length, bpm));
    setCurrentRangeX(0.0, beatsToSeconds(length, bpm));
  }

  // pretend a resizing in order to adjust the sizes of the freshly created TrackComponents:
  resized();

  trackComponents.getLock().exit();
}

void MixsonicArrangementPanel::setTempoInBpm(double newTempo)
{
  TimeUnitConverter::setGlobalTempoInBpm(newTempo);
  timeLine->repaint();
}

void MixsonicArrangementPanel::setTimeUnit(int newTimeUnit)
{
  arrangementToEdit->setSelectedTimeUnit(newTimeUnit);
  timeLine->setSelectedTimeUnit(newTimeUnit);
  trackComponents.getLock().enter();
  for(int t=0; t<trackComponents.size(); t++)
    trackComponents[t]->bodyComponent->setSelectedTimeUnit(newTimeUnit);
  trackComponents.getLock().exit();
}

void MixsonicArrangementPanel::setDescriptionField(Label* newDescriptionField)
{
  descriptionField = newDescriptionField;

  trackComponents.getLock().enter();
  for(int t=0; t<trackComponents.size(); t++)
    trackComponents[t]->bodyComponent->setDescriptionField(descriptionField);
  trackComponents.getLock().exit();
}

void MixsonicArrangementPanel::setComponentGrabber(ComponentGrabber *newGrabber)
{
  grabber = newGrabber;
  trackComponents.getLock().enter();
  for(int t=0; t<trackComponents.size(); t++)
    trackComponents[t]->bodyComponent->setComponentGrabber(newGrabber);
  trackComponents.getLock().exit();
}

void MixsonicArrangementPanel::adjustTimeCursorPosition()
{
  double t = arrangementToEdit->getCurrentTimeInBeats();
  int    x = (int) round(timeLine->beatsToPixelPosition(t)) + timeLine->getX();
  int    h = trackComponents[trackComponents.size()-1]->getBottom();

  //if( x < trackComponents[0]->getTrackBodyOffset() && x > getWidth() )
  if( x < MixsonicTrackControlComponent::getDesiredWidth() || x > getWidth() )
  {
    timeCursor->setVisible(false);
  }
  else
  {
    timeCursor->setVisible(true);
    timeCursor->setBounds(x, 0, 1, h);  
  }
  // \todo it's not a good idea to access elements of the trackComponents array here as we don't 
  // acquire locks - we should keep the desired height as a member and the getTrackBodyOffset call
  // should be replaced by accessing some global variable
}

void MixsonicArrangementPanel::updateGuiAccordingToPlaybackPosition()
{
  timeLine->setCurrentTimeInSamples((double)arrangementToEdit->positionInSamples);  

  double t; 
  int    x;
  if( arrangementToEdit->selectedTimeUnit == TimeUnitConverter::BEATS )
  {
    t = arrangementToEdit->getCurrentTimeInBeats();
    x = (int) round(timeLine->beatsToPixelPosition(t));
  }
  else
  {
    t = arrangementToEdit->getCurrentTimeInSeconds();
    x = (int) round(timeLine->secondsToPixelPosition(t));
  }

  //int    h = trackComponents[trackComponents.size()-1]->getBottom();

  if( autoScrollActive && x > (int) (autoScrollThreshold * getWidth()) )
  {
    double scrollOffset;
    double tMin  = getCurrentRangeMinX();
    double tMax  = getCurrentRangeMaxX();
    double range = tMax-tMin;

    // add an offset to the maximum visible time corresponding to the number of pixels which each
    // scroll step should take:
    scrollOffset = autoScrollStepInPixels * (range / (double) getWidth() );
    tMax         = tMax+scrollOffset;

    // make sure that tis step at least brings us below the threshold (the factor of 2.0 serves
    // for some margin):
    tMax = jmax(tMax, t+2.0*(1.0-autoScrollThreshold)*range);

    // clip tMax at the end of the arrangement:
    tMax = jmin(tMax, getMaximumRangeMaxX());

    // adjust the minum visible time accordingly (so as to maintain the range's size) and set up 
    // the new range:
    tMin = tMax-range;
    setCurrentRangeX(tMin, tMax);
       // setCurrentRangeX will also call adjustTimeCursorPosition()
  }
  else
    adjustTimeCursorPosition();
}

void MixsonicArrangementPanel::setDrawingThread(TimeSliceThread *newDrawingThread)
{
	//Panel::setDrawingThread(newDrawingThread);
  drawingThread = newDrawingThread;
  trackComponents.getLock().enter();
  for(int t=0; t<trackComponents.size(); t++)
    trackComponents[t]->bodyComponent->setDrawingThread(newDrawingThread);
  trackComponents.getLock().exit();
}

void MixsonicArrangementPanel::setTrackHeight(int newHeight)
{
  if( newHeight != trackHeight && newHeight >= TrackComponent::getMinimumHeight() )
  {
    trackHeight = newHeight;
    updatePanelRangeY();
    setupTrackBounds();
  }
}

//-------------------------------------------------------------------------------------------------
// inquiry:

int MixsonicArrangementPanel::getDesiredClipWidth(Clip *theClip) const
{
  return timeLine->getDesiredClipWidth(theClip);
}

bool MixsonicArrangementPanel::canLassoSelectBeginHere(int x, int y) const
{
  if( x > getWidth() )
    return false;
  if( y < timeLine->getBottom() )
    return false;

  trackComponents.getLock().enter();
  int t = getClosestTrackComponent(y);
  if( t != -1 )
  {
    bool result = trackComponents[t]->canLassoSelectBeginHere(x);
    trackComponents.getLock().exit();
    return result;
  }
  else
  {
    trackComponents.getLock().exit();
    return true;
  }
}

bool MixsonicArrangementPanel::isCurrentTimeBehindEndOfArrangement()
{
  double t    = arrangementToEdit->getCurrentTimeInSeconds() 
                * TimeUnitConverter::getSystemSampleRate(); // maybe get rid of TimeUnitConverter::
  double tMax = (double) arrangementToEdit->getTotalLength();  
  if( t > tMax )
    return true;
  else
    return false;
}

int MixsonicArrangementPanel::getTrackAreaY() const
{
  return timeLine->getBottom();
}

int MixsonicArrangementPanel::getHeightForAllTracks() const 
{ 
  return trackHeight * arrangementToEdit->getNumTracks(); 
}

//-------------------------------------------------------------------------------------------------
// callbacks:

void MixsonicArrangementPanel::samplePathHasChanged(int index, const File &oldFilePath, 
                                            const File &newFilePath)
{

}

void MixsonicArrangementPanel::sampleIsToBeDeleted(int index)
{
  trackComponents.getLock().enter();
  for(int t=0; t<trackComponents.size(); t++)
    trackComponents[t]->bodyComponent->refreshClipComponents();
  trackComponents.getLock().exit();
}

void MixsonicArrangementPanel::mouseDown(const MouseEvent &e)
{
  // because we intercept mouse-events of several child components, we first cast the event to our 
  // own coordinate system:
  MouseEvent e2 = e.getEventRelativeTo(this);

  // for clicks inside the empty area, we begin opening a lasso selection:
  if( canLassoSelectBeginHere(e2.x, e2.y) )
  {
    // without shift, we first de-select all clips:
    if( !e.mods.isShiftDown() )
      selectAllClips(false);

    lassoRectangle = new RectangleComponent(Colours::lightgrey.withAlpha(0.1f), Colours::lightgrey);
    addAndMakeVisible(lassoRectangle);
    lassoRectangle->setBounds(e2.x, e2.y, 0, 0);
  }
}

void MixsonicArrangementPanel::mouseDrag(const MouseEvent &e)
{
  MouseEvent e2 = e.getEventRelativeTo(this);

  // update the lasso:
  if( lassoRectangle != NULL )
  {
    int xStart = e2.getMouseDownX();
    int yStart = e2.getMouseDownY();
    int w      = e2.x-xStart;
    int h      = e2.y-yStart;
    if( w >= 0 && h >= 0 )
      lassoRectangle->setBounds(xStart, yStart, w, h);
    else if( w >= 0 && h < 0 )
      lassoRectangle->setBounds(xStart, yStart+h, w, -h);
    else if( w < 0 && h >= 0 )
      lassoRectangle->setBounds(xStart+w, yStart, -w, h);
    else
      lassoRectangle->setBounds(xStart+w, yStart+h, -w, -h);

    // update the temporary selection states of all clips:
    clearTemporarySelectionStates();  
    selectClipsInRectangle(lassoRectangle->getBounds(), true);
  }
}

void MixsonicArrangementPanel::mouseUp(const MouseEvent &e)
{
  if( lassoRectangle != NULL )
  {
    clearTemporarySelectionStates();
    selectClipsInRectangle(lassoRectangle->getBounds(), false);
    removeChildComponent(lassoRectangle);
    delete lassoRectangle;
    lassoRectangle = NULL;
  }
}

void MixsonicArrangementPanel::changeListenerCallback(ChangeBroadcaster *objectThatHasChanged)
{
  if( objectThatHasChanged == this )
    return; // nothing to do if this is a self-sent message (might occur from base-class Panel)

  if( objectThatHasChanged == timeLine )
  {
    int timeInSamples = 
      (int) (timeLine->getCurrentTimeInSeconds() * timeLine->getSystemSampleRate());
    arrangementToEdit->setNextReadPosition(timeInSamples);
    adjustTimeCursorPosition();

  }  
  sendChangeMessage(); // informs the MixsonicContentComponent to update its time-field
}

void MixsonicArrangementPanel::selectionStateChangedByMouse(SelectableItem *itemThatHasChanged, 
                                                    const MouseEvent &e)
{
  // when a clip was clicked without holding down shift, we deselect all other clips:
  if( !e.mods.isShiftDown() ) // && itemThatHasChanged->isSelected() )
  {
    bool state = itemThatHasChanged->isSelected();
    selectAllClips(false);

    // the call above has deselected the newly selected item also, therefore we must put it back
    // into it's old state state:
    itemThatHasChanged->setSelected(state, false);
  }
}

//-------------------------------------------------------------------------------------------------
// user interface related functions:

void MixsonicArrangementPanel::clearTemporarySelectionStates()
{
  trackComponents.getLock().enter();
  for(int t=0; t<trackComponents.size(); t++)
    trackComponents[t]->bodyComponent->clearTemporarySelectionStates();
  trackComponents.getLock().exit();
}

void MixsonicArrangementPanel::selectAllClips(bool shouldBeSelected)
{
  trackComponents.getLock().enter();
  for(int t=0; t<trackComponents.size(); t++)
    trackComponents[t]->bodyComponent->selectAllClips(shouldBeSelected);
  trackComponents.getLock().exit();
}

void MixsonicArrangementPanel::clearTrackComponents()
{
  trackComponents.getLock().enter();
  for(int t=0; t<trackComponents.size(); t++)
  {
    removeChildComponent(trackComponents[t]);
    delete trackComponents[t];
  }
  trackComponents.clear();
  trackComponents.getLock().exit();
}

void MixsonicArrangementPanel::createTrackComponents()
{
  trackComponents.getLock().enter();
  arrangementToEdit->tracks.getLock().enter();
  TrackComponent* tc;
  for(int t=0; t<arrangementToEdit->tracks.size(); t++)
  {
    addAndMakeVisible( tc = new TrackComponent(skin, arrangementToEdit->tracks[t]) );
    tc->bodyComponent->setComponentGrabber(grabber);
    //tc->setMouseMode(mouseMode); //deprecated
    tc->bodyComponent->setDescriptionField(descriptionField);
    tc->bodyComponent->setDrawingThread(drawingThread);
    trackComponents.add(tc);
  }
  arrangementToEdit->tracks.getLock().exit();
  trackComponents.getLock().exit();
}

void MixsonicArrangementPanel::updateTrackComponents()
{
  trackComponents.getLock().enter();
  for(int t=0; t<trackComponents.size(); t++)
    trackComponents[t]->bodyComponent->updateClipComponents();
  trackComponents.getLock().exit();
}

bool MixsonicArrangementPanel::dropComponent(Component* droppedComponent, int x, int y)
{
  // check, whether the droppedComponent is of subclass AudioClipComponent via dynamic_cast - if
  // so, we delegate to the dropAudioClipComponent function:
  AudioClipComponent* newClipComponent = dynamic_cast<AudioClipComponent*>(droppedComponent);
  if( newClipComponent != NULL )
    return dropAudioClipComponent(newClipComponent, x, y);

  // the droppedComponent is not of type AudioClipComponent, so it's probably a 
  // ComponentDragContainer that contains a number of clips (if not, return and report failure)
  ComponentDragContainer* container = dynamic_cast<ComponentDragContainer*>(droppedComponent);
  if( container == NULL )
    return false;

  // \todo move the following stuff into a dropComponentDragContainer() function for consistency 
  // with the stuff above:

  // loop through the child-elements of the container (use a while-loop instead of a for-loop, 
  // because the body of the loop changes the number of the container's child components):
  selectAllClips(false);
  bool success;
  Component* child;
  while( container->getNumChildComponents() > 0 )
  {
    child            = container->getChildComponent(0);
    newClipComponent = dynamic_cast<AudioClipComponent*>(child);

    // try to drop the ClipComponent:
    if( newClipComponent != NULL )
    {
      // convert coordinates:
      Point<int> p = getLocalPoint(container, newClipComponent->getPosition());
      p.y += newClipComponent->getHeight()/2; 
      success = dropAudioClipComponent(newClipComponent, p.x, p.y);
    }
    else
      success = false;

    // in case of no success, we are in charge to delete the component:
    if( success == false )
    {
      if( newClipComponent != NULL )
        newClipComponent->deleteUnderlyingClip();
      container->removeChildComponent(child);
      delete child;
    }
  }

  // the container's child componenets are now all either taken over by some track or else were 
  // deleted - now it's time to delete the container itself and report success:
  delete container;
  return true;
}

bool MixsonicArrangementPanel::dropAudioClipComponent(AudioClipComponent *droppedClipComponent, int x, int y)
{
  // try to drop the droppedComponent onto a track, the TrackComponent will either accept or 
  // reject the droppedComponent and inform about that in its boolean return value: 
  trackComponents.getLock().enter();
  int trackIndex = getClosestTrackComponent(y); 
  if( trackIndex < 0 || trackIndex >= trackComponents.size() )
  {
    trackComponents.getLock().exit();
    return false;
  }
  bool wasTakenOver = trackComponents[trackIndex]->dropComponent(
    droppedClipComponent, x-trackComponents[trackIndex]->getX() );
  trackComponents.getLock().exit();

  if( wasTakenOver == true )
  {
    droppedClipComponent->prepareToPlay(TimeUnitConverter::systemSampleRate);
    droppedClipComponent->addSelectionListener(this); // necesarry??
    sendChangeMessage(); // the app must take care of unsaved changes - \todo maybe use an 
                         // arrangementChanged() callback instead for clarity - maybe this
                         // can be called from a trackChanged callback which in turn may get 
                         // invoked from a clipChanged callback, ...
    return true;
  }
  else
    return false;
}

ComponentDragContainer* MixsonicArrangementPanel::grabSelectedClips(bool grabCopy)
{
  trackComponents.getLock().enter();
  int t, c;
    
  // store the selected ClipComponents from all TrackComponents in a local array:
  Array<AudioClipComponent*> clipsOnCurrentTrack;
  Array<AudioClipComponent*> allClips;
  for(t=0; t<trackComponents.size(); t++)
  {
    clipsOnCurrentTrack = trackComponents[t]->bodyComponent->grabSelectedClips(grabCopy);
    allClips.addArray(clipsOnCurrentTrack);
  }

  // infer the enclosing rectangle of all the selected clips and convert their bounds to the 
  // coordinates with respect to this rectangle:
  Rectangle<int> enclosingRectangle = getEnclosingRectangle(allClips);
  convertClipBoundsToRectangle(allClips, enclosingRectangle);

  // create the container, set up its bounds and 
  ComponentDragContainer* returnedContainer = new ComponentDragContainer();
  returnedContainer->setBounds(enclosingRectangle);

  returnedContainer->setOutlineColor(skin->outlineColorHighlight);
  // use skin->outlin...

  // add the ClipComponents to it as child components
  for(c=0; c<allClips.size(); c++)  
    returnedContainer->addAndMakeVisible(allClips[c]);

  trackComponents.getLock().exit();
  return returnedContainer;
}

void MixsonicArrangementPanel::cutSelectedClipsIntoClipboard(bool forgetOldClipboardContents)
{
  arrangementToEdit->cutSelectedClipsIntoClipboard(forgetOldClipboardContents);
  updateTrackComponents();
}

void MixsonicArrangementPanel::copySelectedClipsIntoClipboard(bool forgetOldClipboardContents)
{
  arrangementToEdit->copySelectedClipsIntoClipboard(forgetOldClipboardContents);
}

void MixsonicArrangementPanel::pasteClipsFromClipboard(double timeInBeats)
{
  arrangementToEdit->pasteClipsFromClipboard(timeInBeats);
  updateTrackComponents();
}

void MixsonicArrangementPanel::deleteSelectedClips()
{
  arrangementToEdit->deleteSelectedClips();
  updateTrackComponents();
}

TrackComponent* MixsonicArrangementPanel::getTrackComponent(int trackIndex)
{
  if( trackIndex < 0 || trackIndex >= trackComponents.size() )
    return NULL;
  else
    return trackComponents[trackIndex];
}

void MixsonicArrangementPanel::setCurrentRangeX(double newMinX, double newMaxX)
{
  // let our baseclass take care of clipping the new values to the permitted range:
  Panel::setCurrentRangeX(newMinX, newMaxX); 

  // retrieve the (now clipped) values again:
  newMinX = getCurrentRangeMinX();  
  newMaxX = getCurrentRangeMaxX();

  // set up the child components:
  timeLine->setTimeRangeInSeconds(newMinX, newMaxX);
  trackComponents.getLock().enter();
  for(int t=0; t< trackComponents.size(); t++)
    trackComponents[t]->bodyComponent->setTimeRangeInSeconds(newMinX, newMaxX);
  trackComponents.getLock().exit();

  adjustTimeCursorPosition();
}

void MixsonicArrangementPanel::setCurrentRangeY(double newMinY, double newMaxY)
{
  Panel::setCurrentRangeY(newMinY, newMaxY); 
  trackOffsetY = getCurrentRangeMinY() / trackHeight;
  setupTrackBounds();
}

void MixsonicArrangementPanel::gotoTimeLinePixelX(int x)
{
  double beat = timeLine->pixelPositionToBeats((double) x);
  arrangementToEdit->setCurrentTimeInBeats(beat);
  adjustTimeCursorPosition();
}

//-------------------------------------------------------------------------------------------------
// appearance stuff:

void MixsonicArrangementPanel::resized()
{
  int x = MixsonicTrackControlComponent::getDesiredWidth();
  int w = getWidth() - x;
  timeLine->setBounds(x, 0, w, 20);
  if( getHeight() > getTrackAreaY() ) // otherwise, we hit a breakpoint when height is too small
    updatePanelRangeY();
  setupTrackBounds();
  adjustTimeCursorPosition();
}

void MixsonicArrangementPanel::paintOverChildren(Graphics& g)
{
  int y = timeLine->getBottom();
  int w = timeLine->getX();
  g.setColour(skin->backgroundColor);
  g.fillRect(0, 0, w, timeLine->getHeight());
  g.setColour(skin->outlineColor);
  g.drawLine(0.f, (float)y, (float)getWidth(), (float)y, 2.f);
}

//-------------------------------------------------------------------------------------------------
// internal functions:

int MixsonicArrangementPanel::getClosestTrackComponent(int y) const
{
  if( y <= timeLine->getBottom() )
    return -1;

  int result      = -1;
  int minDistance = INT_MAX;
  int yTrack, distance;
  trackComponents.getLock().enter();
  for(int i=0; i<trackComponents.size(); i++)
  {
    yTrack   = trackComponents[i]->getY() + trackComponents[i]->getHeight()/2;
    distance = abs(y-yTrack);
    if( distance < minDistance )
    {
      minDistance = distance;
      result      = i;
    }
  }
  trackComponents.getLock().exit();
  return result;
}

void MixsonicArrangementPanel::setupTrackBounds()
{ 
  int x = 0;
  int w = getWidth();
  int y = timeLine->getBottom();
  trackComponents.getLock().enter();
  for(int i = 0; i < trackComponents.size(); i++)
  {
    y = (int) (timeLine->getBottom() + (i-trackOffsetY)*trackHeight);
    trackComponents[i]->setBounds(x, y, w, trackHeight);
  }
  trackComponents.getLock().exit();
}

Rectangle<int> MixsonicArrangementPanel::getEnclosingRectangle(juce::Array<AudioClipComponent*> clipComponents)
{
  trackComponents.getLock().enter();

  int left   = INT_MAX;
  int right  = 0; 
  int top    = INT_MAX; 
  int bottom = 0;
  int cLeft, cRight, cTop, cBottom;
  AudioClipComponent* clip;
  TrackComponent*     parentTrack;
  for(int c=0; c<clipComponents.size(); c++)
  {
    clip    = clipComponents[c];

    // \todo maybe we should initialize just with cLeft = clip->getX(); cRight = clip->getRight(); ?
    cLeft   = (int) timeLine->beatsToPixelPosition(clip->getActiveRegionStart(TimeUnitConverter::BEATS, false) );
    cRight  = (int) timeLine->beatsToPixelPosition(clip->getActiveRegionEnd(  TimeUnitConverter::BEATS, false) );
    cTop    = clip->getY();
    cBottom = clip->getBottom();

    int  t = clip->getHostingTrackIndex();
    jassert( t >= 0 && t < trackComponents.size() ); // should never be violated

    parentTrack  = trackComponents[t];
    Point<int> p = getLocalPoint(parentTrack, Point<int>(cLeft, cTop));
    cLeft        = p.x + parentTrack->getTrackBodyOffset();
    cTop         = p.y;
    p            = getLocalPoint(parentTrack, Point<int>(cRight, cBottom));
    cRight       = p.x + parentTrack->getTrackBodyOffset();
    cBottom      = p.y;

    if( cLeft < left )
      left = cLeft;
    if( cRight > right )
      right = cRight;
    if( cTop < top )
      top = cTop;
    if( cBottom > bottom )
      bottom = cBottom;
  }
  if( left == INT_MAX )
    left = 0;
  if( top == INT_MAX )
    top = 0;
  int width  = right-left;
  int height = bottom-top;

  trackComponents.getLock().exit();

  return Rectangle<int>(left, top, width, height);
}

void MixsonicArrangementPanel::convertClipBoundsToRectangle(Array<AudioClipComponent*> clipComponents, 
                                                            Rectangle<int> rectangle)
{
  trackComponents.getLock().enter();

  AudioClipComponent* clip;
  TrackComponent*     parentTrack;
  int x, y;
  for(int c=0; c<clipComponents.size(); c++)
  {
    clip = clipComponents[c];
    x    = (int) timeLine->beatsToPixelPosition(clip->getActiveRegionStart(TimeUnitConverter::BEATS, false) );
    y    = clip->getY(); 

    int t = clip->getHostingTrackIndex();
    jassert( t >= 0 && t < trackComponents.size() );

    parentTrack = trackComponents[t];
    Point<int> p = getLocalPoint(parentTrack, Point<int>(x, y)); 
    x = p.x - rectangle.getX() + parentTrack->getTrackBodyOffset();
    y = p.y - rectangle.getY();

    clip->setTopLeftPosition(x, y);
  }

  trackComponents.getLock().exit();
}

void MixsonicArrangementPanel::selectClipsInRectangle(Rectangle<int> selectionRectangle,  bool temporarily)
{
  if( trackComponents.size() < 1 )
    return;
  trackComponents.getLock().enter();

  int x1          = selectionRectangle.getX()      - trackComponents[0]->getX();
  int x2          = selectionRectangle.getRight()  - trackComponents[0]->getX();
  int y1          = selectionRectangle.getY()      - timeLine->getHeight();
  int y2          = selectionRectangle.getBottom() - timeLine->getHeight();
  int trackHeight = trackComponents[0]->getHeight();
  int minTrack    = jmax(y1 / trackHeight, 0);
  int maxTrack    = jmin(y2 / trackHeight, trackComponents.size()-1);
  for(int t=minTrack; t<=maxTrack; t++)
    trackComponents[t]->selectClipsInPixelRegion(true, x1, x2, temporarily);

  trackComponents.getLock().exit();
}

void MixsonicArrangementPanel::updatePanelRangeY()
{
  int trackAreaY      = getTrackAreaY();
  int availableHeight = getHeight() - trackAreaY; // height available for the tracks
  int requiredHeight  = getHeightForAllTracks();

  Panel::setMaximumRangeY(0.0, requiredHeight);

  MixsonicArrangementEditor *editor 
    = dynamic_cast<MixsonicArrangementEditor*>(getParentComponent());
  jassert( editor != nullptr );

  if( requiredHeight > availableHeight )
  {
    // attempt to grow to required height, if not possible, grow as much as possible:
    editor->panelWantsSizeChange(this, getWidth(), requiredHeight + trackAreaY);
    availableHeight = getHeight() - trackAreaY;
    double startOld = trackOffsetY * trackHeight;
    double startNew = jmin(startOld, (double) (requiredHeight-availableHeight));
    Panel::setCurrentRangeY(startNew, startNew + availableHeight);
    trackOffsetY = startNew / trackHeight;
  }
  else if( requiredHeight < availableHeight )
  {
    // shrink size, such that all track fit exactly:
    editor->panelWantsSizeChange(this, getWidth(), requiredHeight + trackAreaY);
    availableHeight = getHeight() - trackAreaY;
    Panel::setCurrentRangeY(0.0, availableHeight);
    trackOffsetY = 0.0;
  }
  else // requiredHeight == availableHeight
  {
    Panel::setCurrentRangeY(0.0, availableHeight);
    trackOffsetY = 0.0;
  }
}

//-------------------------------------------------------------------------------------------------
// song recall:

XmlElement* MixsonicArrangementPanel::getStateAsXml(const File& songDirectory) const
{
  XmlElement* xmlState = arrangementToEdit->getStateAsXml(songDirectory);

  // add attributes for the length and currently visible section of the arrangement:
  xmlState->setAttribute("firstVisibleBeat",   timeLine->getMinTimeInBeats());
  xmlState->setAttribute("lastVisibleBeat",    timeLine->getMaxTimeInBeats());
  xmlState->setAttribute("timeCursorPosition", arrangementToEdit->getCurrentTimeInBeats() );
  //...

  return xmlState;
}

void MixsonicArrangementPanel::setStateFromXml(const XmlElement &xmlState, const File &songDirectory)
{
  trackComponents.getLock().enter();

  // clear the old, obsolete array of TrackComponents:
  clearTrackComponents();  

  // restore the actual underlying Arrangement:
  arrangementToEdit->setStateFromXml(xmlState, songDirectory);
    
  // for each Track inside Arrangement, create TrackComponent and a MixsonicTrackControlComponent:
  createTrackComponents();

  // let each TrackComponent create its child components for the clips:
  for(int t=0; t<trackComponents.size(); t++)
    trackComponents[t]->bodyComponent->refreshClipComponents();

  // update the time cursor:
  double currentBeat = xmlState.getDoubleAttribute("timeCursorPosition", 0.0);
  arrangementToEdit->setCurrentTimeInBeats(currentBeat);
  adjustTimeCursorPosition();

  // update the visible range:
  double length           = arrangementToEdit->getMaxTimeInBeats();
  double bpm              = arrangementToEdit->getGlobalTempoInBpm();
  double firstVisibleBeat = xmlState.getDoubleAttribute("firstVisibleBeat", 0.0);
  double lastVisibleBeat  = xmlState.getDoubleAttribute("lastVisibleBeat",  length);
  timeLine->setTimeRangeInBeats(firstVisibleBeat, lastVisibleBeat);
  if( arrangementToEdit->selectedTimeUnit == TimeUnitConverter::BEATS )
  {
    setMaximumRangeX(0.0, length);
    setCurrentRangeX(firstVisibleBeat, lastVisibleBeat);
  }
  else // SECONDS
  {
    setMaximumRangeX(0.0, beatsToSeconds(length, bpm));
    setCurrentRangeX(beatsToSeconds(firstVisibleBeat, bpm), beatsToSeconds(lastVisibleBeat, bpm));
  }

  // pretend a resizing in order to adjust the sizes of the freshly created TrackComponents:
  resized();

  trackComponents.getLock().exit();
}