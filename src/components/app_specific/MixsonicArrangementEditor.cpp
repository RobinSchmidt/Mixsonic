#include "MixsonicArrangementEditor.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction:

MixsonicArrangementEditor::MixsonicArrangementEditor(Arrangement* newArrangementToEdit, 
                                     SamplePool* samplePoolToUse) 
: Component(String("MixsonicArrangementEditor"))
{

  // create the headline label:
  addAndMakeVisible( headline = new RLabel(String("HeadlineLabel"), arrangerHeadlineStr) );
  headline->setFont(Font(20, Font::bold));
  //headline->setColour(Label::textColourId, textColor);
  headline->setJustificationType(Justification::centred);

  addAndMakeVisible( arrangementNavigator = new ArrangementScroller() );
  addAndMakeVisible( arrangementPanel     
    = new MixsonicArrangementPanel(newArrangementToEdit, samplePoolToUse) );
  arrangementPanel->addMouseListener(this, true);
  arrangementPanel->addChangeListener(this);
  arrangementPanel->addMouseListener(arrangementNavigator, true);
  arrangementPanel->setComponentGrabber(this);

  // set up the navigator - the constructor and addAndMakeVisible was pulled before the constructor 
  // of the ArrangementPanel in order to have it in front - in order to intercept mousewheel? can
  // this be done more elegantly? yes - it should be possible to create the both first and then 
  // call addAndMakeVisible in the desired order...

  arrangementNavigator->assignPanel(arrangementPanel);

  draggedComponent = NULL;    
  descriptionField = NULL;
}

MixsonicArrangementEditor::~MixsonicArrangementEditor()
{
  deleteAllChildren();
}

//-------------------------------------------------------------------------------------------------
// setup:

void MixsonicArrangementEditor::setDescriptionField(RLabel* newDescriptionField)
{
  descriptionField = newDescriptionField;
  arrangementPanel->setDescriptionField(newDescriptionField);

  //arrangementNavigator->setDescriptionField(newDescriptionField); 
    // implement this - derive from describedComponent - or whatever this class is called
}

void MixsonicArrangementEditor::setDrawingThread(TimeSliceThread *newDrawingThread)
{
  arrangementPanel->setDrawingThread(newDrawingThread);
}

//-------------------------------------------------------------------------------------------------
// inquiry:

int MixsonicArrangementEditor::getDesiredClipWidth(Clip *theClip) const
{
  return arrangementPanel->getDesiredClipWidth(theClip);
}

//-------------------------------------------------------------------------------------------------
// callbacks:

void MixsonicArrangementEditor::mouseUp(const MouseEvent &e)
{
  if( draggedComponent != NULL )
  {
    // obtain a MouseEvent with respect to the arrangementPanel child-component and call back the 
    // arrangementPanel with the dropped component:
    MouseEvent e2              = e.getEventRelativeTo(arrangementPanel);
    bool componentWasTakenOver = false;  
    componentWasTakenOver      = arrangementPanel->dropComponent(draggedComponent, e2.x, e2.y);

    // either the embedded arrangementPanel has taken over the component in which case we only 
    // clear our references to the objects, or it has not taken over, in which case we also 
    // delete the object:
    removeChildComponent(draggedComponent);
    if( componentWasTakenOver == false )
      delete draggedComponent;
    draggedComponent = NULL; 
    adjustArrangementLength();
  }
}

void MixsonicArrangementEditor::mouseDrag(const MouseEvent &e)
{
  if( draggedComponent != NULL )
    componentDragger.dragComponent(draggedComponent, e, NULL); 
}

void MixsonicArrangementEditor::buttonClicked(Button *buttonThatWasClicked)
{
  /*
  // currently not used, but kept for reference, if this function should come back again (via 
  // different GUI means):
  if( buttonThatWasClicked == duplicateButton )
  {
    arrangementPanel->copySelectedClipsIntoClipboard();
    arrangementPanel->pasteClipsFromClipboard(
      arrangementPanel->arrangementToEdit->getLastClipEndTimeInBeats(true));
    adjustArrangementLength();
    return;
  }
  */



  // todo: handle these callbacks in TrackControlComponent: 
  /*
  trackControlComponents.getLock().enter();  

  // find out if it was one of the solo-buttons inside the MixsonicTrackControlComponents:
  int soloedTrack = -1;
  for(int t=0; t<trackControlComponents.size(); t++)
  {
    if( buttonThatWasClicked == trackControlComponents[t]->soloButton )
    {
      //setSongFileUpToDateFlag(false);
      if( trackControlComponents[t]->soloButton->getToggleState() == true )
        soloedTrack = t;
    }
  }

  // if a solo-button was toggled on, toggle off the solo buttons of all the other tracks:
  if( soloedTrack != -1 )
  {
    for(int t=0; t<trackControlComponents.size(); t++)
    {
      if( t != soloedTrack )
        trackControlComponents[t]->soloButton->setToggleState(false, false);
    }
  }
  trackControlComponents.getLock().exit();

  // tell the arrangement which track is in solo mode:
  arrangementPanel->arrangementToEdit->setTrackSolo(soloedTrack);
  */




  sendChangeMessage();
}

void MixsonicArrangementEditor::changeListenerCallback(ChangeBroadcaster *objectThatHasChanged)
{
  sendChangeMessage(); 
}

void MixsonicArrangementEditor::timerCallback()
{
  // this callback will be called when the app is playing - in this case, we need to trigger a 
  // repaint at regular intervals in order to update the position of the time-cursor:
  arrangementPanel->updateGuiAccordingToPlaybackPosition();
  arrangementNavigator->updateScrollbars();

  // stop playback when end of the arrangement reached:
  if( arrangementPanel->isCurrentTimeBehindEndOfArrangement() )
  {
    //playButton->setToggleState(false, true); // old
    // \todo mixsonicAppController.stopPlayback(); 
      // we need a global object mixsonicAppController which maintains application-wide settings
      // and can handle application-wide accessible callbacks
  }
}

bool MixsonicArrangementEditor::componentRequestsToBeGrabbed(
  GrabbableComponent* componentToBeGrabbed, MouseEvent grabEvent)
{
  // the mouse event comes from another component, so we first cast the event to our own 
  // coordinate system:
  MouseEvent e  = grabEvent;          // as abbreviation for convenience
  MouseEvent e2 = e.getEventRelativeTo(this);

  // we need to check whether the pointer to draggedComponent is NULL - it may be not when the 
  // user clicks on a position where more than one clips are overlapping (each of them will give
  // rise to it's own mouseDown call, which we intercept here):
  if( draggedComponent == NULL )
  {
    // grab some clip or a copy therof:
    bool grabCopy    = e.mods.isAltDown();
    MouseEvent e3    = e.getEventRelativeTo(arrangementPanel);
    draggedComponent = arrangementPanel->grabSelectedClips(grabCopy); 

    if( draggedComponent != NULL )
    {
      addAndMakeVisible(draggedComponent);            // this is now our child-component
      draggedComponent->addMouseListener(this, true); // we must receive its mouseUp events

      // the bounds of the container were set up with respect to the ArrangementPanel, we need to
      // convert them to our own coordinate system, then start the dragging:
      int x = draggedComponent->getX();
      int y = draggedComponent->getY();
      Point<int> p = getLocalPoint(arrangementPanel, Point<int>(x,y));
      draggedComponent->setTopLeftPosition(p);
      componentDragger.startDraggingComponent(draggedComponent, e2);  
    }
  }
  return true;
}


bool MixsonicArrangementEditor::dropComponent(Component *droppedComponent, int x, int y)
{
  AudioClipComponent* newClipComponent = dynamic_cast<AudioClipComponent*>(droppedComponent);
  if( newClipComponent != NULL )
  {
    newClipComponent->setComponentGrabber(this);
    x -= arrangementPanel->getX();  
    y -= arrangementPanel->getY();
    bool result = arrangementPanel->dropComponent(droppedComponent, x, y);
    if( result == true )
      adjustArrangementLength();
    return result;
  }
  else
    return false;
}

//-------------------------------------------------------------------------------------------------
// appearance stuff:

void MixsonicArrangementEditor::resized()
{
  //headline->setBounds(0, 0, w, 28);

  //setupArrangementPanelBounds();

  int scrollbarThickness = arrangementNavigator->getWidgetSize();
  int numTracks          = arrangementPanel->arrangementToEdit->getNumTracks();
  int trackHeight        = arrangementPanel->getTrackHeight();
  int desiredHeight      = numTracks * trackHeight + arrangementPanel->timeLine->getHeight();
  setupArrangementPanelBounds(getWidth(), desiredHeight); 
    // the width will actually be a bit smaller due to margins and scrollbars, the height might
    // be smaller, if there's not enough space

  /*
  int x = 0;
  int y = 0;
  int w = getWidth();
  int h = getHeight();

  int margin = 5;
  int scrollbarThickness = arrangementNavigator->getWidgetSize();
  int numTracks = arrangementPanel->arrangementToEdit->getNumTracks();
  int trackHeight = arrangementPanel->getTrackHeight();

  x = x+margin;
  y = 0;
  w = w-2*margin-scrollbarThickness;
  //h = getHeight() - y - scrollbarThickness;
  h = numTracks * trackHeight + arrangementPanel->timeLine->getHeight() - y;
  h = jmin(h, getHeight() - y - scrollbarThickness);

  setupArrangementPanelBounds(w, h);

  arrangementPanel->setBounds(x, y, w, h);      
  arrangementNavigator->alignWidgetsToPanel();
  */

  // \todo set up a maximum height for the arrangementPanel and let the arrangementPanel resize 
  // itself when there are not enough tracks - maybe in setTrackHeight and add/removeTrack, etc.
  // perhaps we should override setBounds in ArrangementPanel - like so:
  //h = numTracks * trackHeight + arrangementPanel->timeLine->getHeight() - y;
  //h = jmin(h, getHeight() - y - scrollbarThickness);
}

void MixsonicArrangementEditor::paintOverChildren(Graphics &g)
{
  //g.fillAll(backgroundColor);
  g.setColour(outlineColor);
  g.drawRect(0.f, 0.f, (float)getWidth(), (float)getHeight(), 2.f);

  //int y = arrangementNavigator->getBottom() - arrangementNavigator->getWidgetSize();
  //g.drawLine(0.f, (float)y, (float)getWidth(), (float)y, 2.f);
    // when zooming in 3 times and the scrolling down, there's an additional bright pixel at the 
    // bottom of the TrackControlComponents - no big deal, but i wonder why this is. this line here 
    // covers this, but it seems like a hack
    // does not yet work right
}

bool MixsonicArrangementEditor::panelWantsSizeChange(Panel *panel, int desiredWidth, 
                                                     int desiredHeight)
{
  jassert( panel == arrangementPanel );
  if( getWidth() == desiredWidth && getHeight() == desiredHeight )
    return true;
  return setupArrangementPanelBounds(desiredWidth, desiredHeight);
}

//-------------------------------------------------------------------------------------------------
// others:

XmlElement* MixsonicArrangementEditor::getStateAsXml(const File& songDirectory) const
{
  return arrangementPanel->getStateAsXml(songDirectory);
}

void MixsonicArrangementEditor::setStateFromXml(const XmlElement &xmlState, const File &songDirectory)
{
  arrangementPanel->setStateFromXml(xmlState, songDirectory);  
  arrangementNavigator->updateScrollbars();
}

void MixsonicArrangementEditor::initializeSong()
{
  logMessage("MixsonicArrangementEditor::initializeSong()");

  // initialize the arrangement (and its GUI):
  arrangementPanel->setComponentGrabber(this);
  arrangementPanel->initializeArrangement();
  arrangementNavigator->updateScrollbars();
}

void MixsonicArrangementEditor::startPlayback(int bufferSize, double sampleRate)
{
  arrangementPanel->arrangementToEdit->prepareToPlay(bufferSize, sampleRate);
  arrangementPanel->arrangementToEdit->play(true);
  startTimer(20); //continuously update time-cursor on GUI 
}

void MixsonicArrangementEditor::stopPlayback()
{
  arrangementPanel->arrangementToEdit->play(false); // this stops playback
  stopTimer();  // doesn't need continuous GUI updates when not playing
}

//-------------------------------------------------------------------------------------------------
// internal functions:

bool MixsonicArrangementEditor::isInsideArrangementPanel(int x, int y) const
{
  if(  x >= arrangementPanel->getX() && x <= arrangementPanel->getRight()
    && y >= arrangementPanel->getY() && y <= arrangementPanel->getBottom() )
  {
    return true;
  }
  else
    return false;
}

void MixsonicArrangementEditor::adjustArrangementLength()
{
  Arrangement* a = arrangementPanel->arrangementToEdit;

  if( a->getLastClipEndTimeInBeats(false) > a->getMaxTimeInBeats()-Arrangement::autoGrowThreshold )
    a->setMaxTimeInBeats(a->getLastClipEndTimeInBeats(false) + Arrangement::autoGrowSize);
  if( a->getLastClipEndTimeInBeats(false) < a->getMaxTimeInBeats()-2*Arrangement::autoGrowThreshold )
    a->setMaxTimeInBeats(a->getLastClipEndTimeInBeats(false) + Arrangement::autoGrowSize);
  if(a->getMaxTimeInBeats() < Arrangement::minLengthInBeats)
    a->setMaxTimeInBeats(Arrangement::minLengthInBeats);

  arrangementPanel->setMaximumRangeX(
    0.0, beatsToSeconds(a->getMaxTimeInBeats(), a->getGlobalTempoInBpm()));   

  // that call will take care of updating the view and the scrollbar:
  arrangementNavigator->zoomX(1.0, 0.5);
}

bool MixsonicArrangementEditor::setupArrangementPanelBounds(int desiredWidth, int desiredHeight)
{
  int margin = 5;
  int scrollbarThickness = arrangementNavigator->getWidgetSize();

  int x = margin;
  int y = 0;
  int w = jmin(desiredWidth, getWidth() - margin - scrollbarThickness - x);
  int h = jmin(desiredHeight, getHeight() - scrollbarThickness - y);

  arrangementPanel->setBounds(x, y, w, h);      
  arrangementNavigator->alignWidgetsToPanel();
  arrangementNavigator->updateScrollbars();

  return w == desiredWidth && h == desiredHeight;
}