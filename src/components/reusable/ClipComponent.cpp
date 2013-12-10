#include "ClipComponent.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction:

ClipComponent::ClipComponent(Clip* newClipToEdit, Skin* skinToUse)
: GrabbableComponent(String("ClipComponent")), ClipManipulator(newClipToEdit)
{
  Component::setName(String("ClipComponent"));
  //mouseMode = MOVE_CLIPS;
  skin = skinToUse;
  initialize();
}

ClipComponent::ClipComponent(const ClipComponent &other) : ClipManipulator(other)
{
  // assign the ComponentGrabber to be the same as in the other ClipComponent:
  grabber = other.grabber;
  skin = other.skin;
  initialize();
}

ClipComponent::~ClipComponent()
{
  ScopedLock sl(clipLock);
  if( clipToEdit != NULL )
  {
    clipToEdit->removeClipDeletionWatcher(this);
    clipToEdit = NULL;
  }
  deleteAllChildren();
}

//-------------------------------------------------------------------------------------------------
// setup:

void ClipComponent::setTimeRangeInComponent(double newMinTimeInSeconds, double newMaxTimeInSeconds)
{
  ScopedLock sl(clipLock);
  if( clipToEdit == NULL )
    return;
  Panel::setMaximumRangeX(newMinTimeInSeconds, newMaxTimeInSeconds);
  Panel::setCurrentRangeX(newMinTimeInSeconds, newMaxTimeInSeconds);
}

void ClipComponent::setTimeRangeToRender(double newMinTimeInSeconds, double newMaxTimeInSeconds)
{

}

void ClipComponent::setSelected(bool shouldBeSelected, bool temporarily,                                      
                                bool sendSelectionStateChangeMessage)
{
  ScopedLock sl(clipLock);
  if( clipToEdit == NULL )
    return;
  if( temporarily == true )
  {
    setTemporarilySelected(shouldBeSelected, sendSelectionStateChangeMessage);
    return;
  }

  if( shouldBeSelected != clipToEdit->isSelected() )
  {
    clipToEdit->setSelected(shouldBeSelected, sendSelectionStateChangeMessage);
    repaint();
  }
}

void ClipComponent::setTemporarilySelected(bool shouldBeTemporarilySelected,                                                
                                           bool sendSelectionStateChangeMessage)
{
  ScopedLock sl(clipLock);
  if( clipToEdit == NULL )
    return;
  if( shouldBeTemporarilySelected != clipToEdit->isTemporarilySelected() )
  {
    clipToEdit->setTemporarilySelected(shouldBeTemporarilySelected, 
      sendSelectionStateChangeMessage);
    repaint();
  }
}

void ClipComponent::setMouseMode(int newMouseMode)
{
  mouseMode = newMouseMode;
}

//-------------------------------------------------------------------------------------------------
// callbacks:

void ClipComponent::clipNameChanged(Clip* clipThatHasChangedItsName)
{
  if( clipThatHasChangedItsName == clipToEdit )  // should always be
    caption.setText(clipToEdit->getName());
  else
    jassertfalse;
}

void ClipComponent::mouseMove(const MouseEvent &e)
{
  MouseEvent e2 = e.getEventRelativeTo(this);
  float x = (float) e2.x;
  float y = (float) e2.y;

  if( isOverLeftBorder(x, y) )
    setMouseCursor( MouseCursor(MouseCursor::LeftEdgeResizeCursor) );
  else if( isOverRightBorder(x, y) )
    setMouseCursor( MouseCursor(MouseCursor::RightEdgeResizeCursor) );
  else
    setMouseCursor( MouseCursor(MouseCursor::NormalCursor) );
}

void ClipComponent::mouseDown(const MouseEvent &e)
{
  ScopedLock sl(clipLock);
  if( clipToEdit == NULL )
    return;

  /*
  // obsolete:
  if( mouseMode == DELETE_CLIPS )
  {
    clipToEdit->sendDeletionRequest();
    return;
  }
  */

  MouseEvent e2 = e.getEventRelativeTo(this);
  float x = (float) e2.x;
  float y = (float) e2.y;

  /*
  // obsolete:
  if( mouseMode == SPLIT_CLIP )
  {
    double splitTime = x;
    double dummy     = y;
    transformFromComponentsCoordinates(splitTime, dummy);
    clipToEdit->sendSplitRequest(splitTime, TimeUnitConverter::SECONDS, true);
    return;
  }
  */

  if( e.mods.isLeftButtonDown() && !e.mods.isShiftDown() )
  {
    if( isOverLeftBorder(x, y) )
      currentlyDraggedHandle = LEFT_BORDER;
    else if( isOverRightBorder(x, y) )
      currentlyDraggedHandle = RIGHT_BORDER;
    else if( isOnMuteButton(x, y) )
    {
      muteButton->setToggleState(!muteButton->getToggleState(), true);
      return;
    }
    else
    {
      currentlyDraggedHandle = NONE;
      if( clipToEdit->isSelected() )
      {
        GrabbableComponent::mouseDown(e); // this will give rise to a grab-request
      }
      else
      {
        clipToEdit->setSelectedByMouse(true, e);
        repaint();
      }
    }
  }
  else if( e.mods.isLeftButtonDown() && e.mods.isShiftDown() )
  {
    clipToEdit->setSelectedByMouse(!clipToEdit->isSelected(), e);
    repaint();
  }
  else if( e.mods.isRightButtonDown() )
  {
    clipToEdit->setSelectedByMouse(false, e);
    repaint();
  }
}

void ClipComponent::mouseDrag(const MouseEvent &e)
{
  ScopedLock sl(clipLock);
  if( clipToEdit == NULL )
    return;
  MouseEvent e2 = e.getEventRelativeTo(this);
  
  float x = (float) e2.x;
  if( currentlyDraggedHandle == LEFT_BORDER )
    x = jmin(x, (float) getWidth());
  else if( currentlyDraggedHandle == RIGHT_BORDER )
    x = jmax(x, 0.f);
  else
    x = jlimit(0.f, (float) getWidth(), x);
  //float y = jlimit(0.f, (float) getHeight(), (float) e2.y);

  if( currentlyDraggedHandle == LEFT_BORDER )
  {
    x = jlimit(0.f, (float) (clipToEdit->getActiveRegionEnd(TimeUnitConverter::SECONDS, true)-0.01), x);
    clipToEdit->setActiveRegionStart(x, TimeUnitConverter::SECONDS, true);  
    sendChangeMessage();
  }
  else if( currentlyDraggedHandle == RIGHT_BORDER )
  {
    x = jlimit((float) (clipToEdit->getActiveRegionStart(TimeUnitConverter::SECONDS, true)+0.01),
               (float) (clipToEdit->getTotalDuration(TimeUnitConverter::SECONDS)), x);
    clipToEdit->setActiveRegionEnd(x, TimeUnitConverter::SECONDS, true);    
    sendChangeMessage();
  }
}

void ClipComponent::mouseUp(const MouseEvent &e)
{
  currentlyDraggedHandle = NONE;
}

void ClipComponent::buttonClicked(Button *buttonThatWasClicked)
{
  ScopedLock sl(clipLock);
  if( clipToEdit == NULL )
    return;
  if( buttonThatWasClicked == muteButton )
    clipToEdit->setMute(muteButton->getToggleState());
  repaint();
}

//-------------------------------------------------------------------------------------------------
// appearance stuff:

void ClipComponent::paintOverChildren(Graphics &g)
{
  ScopedLock sl(clipLock);
  if( clipToEdit == NULL )
    return;

  // draw the cross-hatch pattern which indicates the mute state:
  if( clipToEdit->isMuted() )
  {
    // old:
    //g.setColour(Skin::getInstance()->markerColor);
    //g.drawLine(0.f, 0.f,                 (float) getWidth(), (float) getHeight(), 2.f);
    //g.drawLine(0.f, (float) getHeight(), (float) getWidth(), 0.f,                 2.f);

    g.setColour(skin->outlineColor);
    drawCrossHatches(g, this, 16.f, 1.f);
  }

  // drw the caption - needs refinement:
  Rectangle<int> r = caption.getBounds();
  float w = (float) r.getWidth();
  float h = (float) r.getHeight();
  if( w < getWidth()-10 )
  {
    g.setColour(Colours::white.withAlpha(0.7f));
    g.fillRect(0.5f*getWidth()-0.5f*w-2.f, 1.f, w+4.f, h);
    g.setColour(Colours::black);
    caption.drawAt(g, 0.5f*getWidth()-0.5f*w, 12, 1.f);
  }

  // draw (or don't draw) the frame which indicates the selection state:
  if( clipToEdit->isSelected() || clipToEdit->isTemporarilySelected() )
  {
    g.setColour(skin->outlineColorHighlight);
    g.drawRect(0, 0, getWidth(), getHeight(), 2);
  }
}

void ClipComponent::resized()
{
  Panel::resized();

  ScopedLock sl(clipLock);
  if( clipToEdit == NULL )
    return;

  if( getWidth() >= 40 && getHeight() >= 32 )
  {
    muteButton->setVisible(true);
    int x = getWidth()-40;
    int y = getHeight()-16;
    muteButton->setBounds(x, y, 40, 16);
  }
  else
    muteButton->setVisible(false);
}

void ClipComponent::paint(Graphics &g)
{
  //Panel::paint(g);
}

void ClipComponent::setMouseCursor(const MouseCursor &cursorType)
{

}

//-------------------------------------------------------------------------------------------------
// internal functions:

void ClipComponent::initialize()
{
  ScopedLock sl(clipLock);

  currentlyDraggedHandle = NONE;

  muteButton = new HalfTransparentButton(clipMuteStr);
  addAndMakeVisible(muteButton);
  muteButton->setDescription(clipMuteHelpStr);
  muteButton->setClickingTogglesState(true);
  muteButton->addListener(this);
  muteButton->setInterceptsMouseClicks(false, false);

  caption.setColour(Colours::black);

  if( clipToEdit != NULL )
  {
    muteButton->setToggleState(clipToEdit->isMuted(), false);
    caption.setText(clipToEdit->getName()); 
  }

  updateDisplay();
}

void ClipComponent::updateDisplay()
{

}

bool ClipComponent::isOnMuteButton(float x, float y)
{
  if( muteButton->isVisible() == false )
    return false;

  if(  x >= muteButton->getX() && x <= muteButton->getRight()
    && y >= muteButton->getY() && y <= muteButton->getBottom() )
  {
    return true;
  }
  else
    return false;
}

bool ClipComponent::isOverLeftBorder(float x, float y)
{
  if( x > 4 || getWidth() < 16  )
    return false;
  else
    return true;
}

bool ClipComponent::isOverRightBorder(float x, float y)
{
  if( x < getWidth()-4 || getWidth() < 16 )
    return false;
  else
    return true;
}