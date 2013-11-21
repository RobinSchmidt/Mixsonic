#include "MixsonicArrangementScroller.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction:

PanelScroller::PanelScroller(Skin *skinToUse, const String &componentName) 
: Component(componentName)
{
  thePanel            = NULL;

  zoomFactorPerClickX = 1.25;
  shiftPerClickX      = 0.1;

  zoomFactorPerClickY = 1.25;
  shiftPerClickY      = 0.1;

  widgetSize          = 16;

  scrollBarX = new ScrollBar(false, true); // make class RScrollBar, pass the (widget)skin
  scrollBarX->setRangeLimits(0.0, 1.0);
  scrollBarX->setAutoHide(false);
  scrollBarX->addListener(this);
  addAndMakeVisible(scrollBarX);

  scrollBarY = new ScrollBar(true, true); 
  scrollBarY->setRangeLimits(0.0, 1.0);
  scrollBarY->setAutoHide(false);
  scrollBarY->addListener(this);
  addAndMakeVisible(scrollBarY);

  zoomOutButtonX = new RButton(skinToUse, RButton::MINUS);
  zoomOutButtonX->setClickingTogglesState(false);
  zoomOutButtonX->addListener(this);
  addAndMakeVisible(zoomOutButtonX);

  zoomToAllButtonX = new RButton(skinToUse, "A");
  zoomToAllButtonX->setClickingTogglesState(false);
  zoomToAllButtonX->addListener(this);
  addAndMakeVisible(zoomToAllButtonX);

  zoomInButtonX = new RButton(skinToUse, RButton::PLUS);
  zoomInButtonX->setClickingTogglesState(false);
  zoomInButtonX->addListener(this);
  addAndMakeVisible(zoomInButtonX);

  zoomOutButtonY = new RButton(skinToUse, RButton::MINUS);
  zoomOutButtonY->setClickingTogglesState(false);
  zoomOutButtonY->addListener(this);
  addAndMakeVisible(zoomOutButtonY);

  zoomToAllButtonY = new RButton(skinToUse, "A");
  zoomToAllButtonY->setClickingTogglesState(false);
  zoomToAllButtonY->addListener(this);
  addAndMakeVisible(zoomToAllButtonY);

  zoomInButtonY = new RButton(skinToUse, RButton::PLUS);
  zoomInButtonY->setClickingTogglesState(false);
  zoomInButtonY->addListener(this);
  addAndMakeVisible(zoomInButtonY);

  zoomToAllButtonXY = new RButton(skinToUse, "A");
  zoomToAllButtonXY->setClickingTogglesState(false);
  zoomToAllButtonXY->addListener(this);
  addAndMakeVisible(zoomToAllButtonXY);
}

PanelScroller::~PanelScroller()
{
  deleteAllChildren();
}

//-------------------------------------------------------------------------------------------------
// setup:

void PanelScroller::assignPanel(Panel* newPanelToAssign)
{
  // assign the pointer to the new Panel:
  thePanel = newPanelToAssign;

  // align the widgets:
  alignWidgetsToPanel();

  // update the scrollbar ranges (this sets up out currentMinX, etc. members as well):
  updateScrollbars();
}

void PanelScroller::invalidatePanelPointer()
{
  thePanel = NULL;
}

//-------------------------------------------------------------------------------------------------
// callbacks:

void PanelScroller::scrollBarMoved(ScrollBar *scrollBarThatHasMoved, 
                                                 double newRangeStart)
{
  // check, if we have a valid pointer to a Panel:
  if( thePanel == NULL ) 
    return;

  double v1, v2, r;
  if( scrollBarThatHasMoved == scrollBarX )
  {
    v1 = newRangeStart;
    r  = scrollBarX->getCurrentRangeSize();
    v2 = v1+r;
    v1 = transformFromScrollBarCoordinateX(v1);
    v2 = transformFromScrollBarCoordinateX(v2);
    thePanel->setCurrentRangeX(v1, v2);
  }
  else if( scrollBarThatHasMoved == scrollBarY )
  {
    v1 = newRangeStart;
    r  = scrollBarY->getCurrentRangeSize();
    v2 = v1+r;
    v1 = transformFromScrollBarCoordinateY(v1);
    v2 = transformFromScrollBarCoordinateY(v2);
    thePanel->setCurrentRangeY(v1, v2);
  }
}

void PanelScroller::buttonClicked(Button *buttonThatWasClicked)
{
  if( buttonThatWasClicked == zoomInButtonX )
    zoomX(zoomFactorPerClickX);
  else if( buttonThatWasClicked == zoomToAllButtonX )
    zoomToAllX();
  else if( buttonThatWasClicked == zoomOutButtonX )
    zoomX(1/zoomFactorPerClickX);
  else if( buttonThatWasClicked == zoomInButtonY )
    zoomY(zoomFactorPerClickY);
  else if( buttonThatWasClicked == zoomToAllButtonY )
    zoomToAllY();
  else if( buttonThatWasClicked == zoomOutButtonY )
    zoomY(1/zoomFactorPerClickY);
  else if( buttonThatWasClicked == zoomToAllButtonXY )
    zoomToAllXY();
}

void PanelScroller::mouseWheelMove(const MouseEvent& e, 
                                   float wheelIncrementX, float wheelIncrementY)
{
  // obtain the zoom focus (the point into which we zoom):
  MouseEvent e2 = e.getEventRelativeTo(this);
  int nw = thePanel->getWidth();  // navigable width
  int nh = thePanel->getHeight(); // navigable height
  double relativeX =        (double) e2.x / (double) nw;
  double relativeY =  1.0 - (double) e2.y / (double) nh;
  relativeX = jlimit(0.01, 0.99, relativeX);
  relativeY = jlimit(0.01, 0.99, relativeY);

  if( wheelIncrementY >= 0.f )
    zoomX(zoomFactorPerClickX, relativeX);
  else if( wheelIncrementY < 0.f )
    zoomX(1.f/zoomFactorPerClickX, relativeX);

  // \todo to make this class more general, we should optionally also zoom vertically and/or let
  // the user select mousewheel behavior (zoom or scroll, vertically and/or horizontally, etc.)

  updateScrollbars();
}

// forwarding of mouse-events to the Panel which are not handled by the zoomer:
void PanelScroller::mouseMove(const MouseEvent& e) 
{ 
  if(thePanel != NULL) 
    thePanel->mouseMove(e); 
}

void PanelScroller::mouseEnter(const MouseEvent& e) 
{ 
  if(thePanel != NULL) 
    thePanel->mouseEnter(e); 
}

void PanelScroller::mouseExit(const MouseEvent& e) 
{ 
  if(thePanel != NULL) 
    thePanel->mouseExit(e); 
}

void PanelScroller::mouseDown(const MouseEvent& e) 
{
  // zoom to all on middle button
  if( e.mods.isMiddleButtonDown() )
    zoomToAllX();
  else if (thePanel != NULL) 
    thePanel->mouseDown(e); 
}

void PanelScroller::mouseDrag(const MouseEvent& e) 
{ 
  if(thePanel != NULL) 
    thePanel->mouseDrag(e); 
}

void PanelScroller::mouseUp(const MouseEvent& e) 
{ 
  if(thePanel != NULL) 
    thePanel->mouseUp(e); 
}

void PanelScroller::mouseDoubleClick(const MouseEvent& e) 
{ 
  if(thePanel != NULL) 
    thePanel->mouseDoubleClick(e); 
}

//-------------------------------------------------------------------------------------------------
// manipulation of the panel:

void PanelScroller::shiftX(double shiftAmount)
{
  if( thePanel == NULL ) 
    return;

  double x1, x2, w;

  // convert the leftmost and rightmost currently visible x-value to normalized coordinates:
  x1 = thePanel->getCurrentRangeMinX();
  x2 = thePanel->getCurrentRangeMaxX();
  x1 = transformToScrollBarCoordinateX(x1);
  x2 = transformToScrollBarCoordinateX(x2);
  w  = scrollBarX->getCurrentRangeSize();

  // shift in the normalized coordinate domain, thereby make sure to keep the thumb-size rigid and 
  // not exceed the boundaries:
  if( shiftAmount >= 0.0 )
  {
    x2 += shiftAmount;
    x2  = jlimit(0.0, 1.0, x2);
    x1  = x2-w;
    x1  = jlimit(0.0, 1.0, x1);
  }
  else
  {
    x1 += shiftAmount;         // shiftAmount is itself negative
    x1  = jlimit(0.0, 1.0, x1);
    x2  = x1+w;
    x2  = jlimit(0.0, 1.0, x2);
  }

  // convert back to unnormalized coordinates and set up the Panel's current range:
  x1 = transformFromScrollBarCoordinateX(x1);
  x2 = transformFromScrollBarCoordinateX(x2);
  thePanel->setCurrentRangeX(x1, x2);
}

void PanelScroller::zoomX(double zoomFactor, double relativeCenter)
{
  // check, if we have a valid pointer to a Panel:
  if( thePanel == NULL ) 
    return;

  double x1, x2, center, left, right;

  // convert the leftmost and rightmost currently visible x-value to normalized coordinates:
  x1 = thePanel->getCurrentRangeMinX();
  x2 = thePanel->getCurrentRangeMaxX();
  x1 = transformToScrollBarCoordinateX(x1);
  x2 = transformToScrollBarCoordinateX(x2);

  // get the current center-value and the deviations from it to the left and right:
  center = x1 + relativeCenter*(x2-x1);
  left   = center-x1;    // deviation to the left
  right  = x2-center;    // deviation to the right

  // obtain the new normalized coordinates for the currently visible range:
  x1     = center - left/zoomFactor;
  x2     = center + right/zoomFactor;

  // convert back to unnormalized coordinates and set up the Panel's current range:
  x1     = transformFromScrollBarCoordinateX(x1);
  x2     = transformFromScrollBarCoordinateX(x2);
  if( x2-x1 >= thePanel->getMinimumWidth() )
    thePanel->setCurrentRangeX(x1, x2);

  // update the x-axis scrollbar according to the new zoom-level:
  updateScrollbars();
}
void PanelScroller::zoomY(double zoomFactor, double relativeCenter)
{
  // copy/paste/edited from zoomX - we need to refactor to get rid of this duplication

  if( thePanel == NULL ) 
    return;

  double y1, y2, center, left, right;

  y1     = thePanel->getCurrentRangeMinY();
  y2     = thePanel->getCurrentRangeMaxY();
  y1     = transformToScrollBarCoordinateY(y1);
  y2     = transformToScrollBarCoordinateY(y2);
  center = y1 + relativeCenter*(y2-y1);
  left   = center-y1;
  right  = y2-center;
  y1     = center - left/zoomFactor;
  y2     = center + right/zoomFactor;
  y1     = transformFromScrollBarCoordinateY(y1);
  y2     = transformFromScrollBarCoordinateY(y2);
  if( y2-y1 >= thePanel->getMinimumWidth() )
    thePanel->setCurrentRangeY(y1, y2);

  updateScrollbars();
}

void PanelScroller::zoomToAllX()
{
  if( thePanel == NULL ) 
    return;
  thePanel->setCurrentRangeX(thePanel->getMaximumRangeMinX(), 
    thePanel->getMaximumRangeMaxX());
  updateScrollbars();
}
void PanelScroller::zoomToAllY()
{
  if( thePanel == NULL ) 
    return;
  thePanel->setCurrentRangeY(thePanel->getMaximumRangeMinY(), 
    thePanel->getMaximumRangeMaxY());
  updateScrollbars();
}
void PanelScroller::zoomToAllXY()
{
  zoomToAllX();
  zoomToAllY();
}

void PanelScroller::alignWidgetsToPanel()
{
  // check, if we have a valid pointer to a Panel:
  if( thePanel == NULL ) 
    return;

  //int widgetSize = 16; // make this a member variable

  setBounds(thePanel->getX(), thePanel->getY(), 
    thePanel->getWidth()+widgetSize, thePanel->getHeight()+widgetSize);

  bool zoomableX = zoomFactorPerClickX != 1.0;;
  int x = 0;
  int y = thePanel->getHeight();
  int w = thePanel->getWidth() - zoomableX * 3 * widgetSize;
  int h = widgetSize;
  scrollBarX->setBounds(x, y, w, h);
  if( zoomableX )
  {
    x = scrollBarX->getRight();
    zoomOutButtonX->setBounds(x, y, widgetSize, widgetSize);
    x += widgetSize;
    zoomToAllButtonX->setBounds(x, y, widgetSize, widgetSize);
    x += widgetSize;
    zoomInButtonX->setBounds(x, y, widgetSize, widgetSize);
  }

  bool zoomableY = zoomFactorPerClickY != 1.0;;
  x = thePanel->getWidth();
  y = 0;
  w = widgetSize;
  h = thePanel->getHeight() - zoomableY * 3 * widgetSize;
  scrollBarY->setBounds(x, y, w, h);
  if( zoomableY )
  {
    y = scrollBarY->getBottom();
    zoomOutButtonY->setBounds(x, y, widgetSize, widgetSize);
    y += widgetSize;
    zoomToAllButtonY->setBounds(x, y, widgetSize, widgetSize);
    y += widgetSize;
    zoomInButtonY->setBounds(x, y, widgetSize, widgetSize);
  }

  if( zoomableX && zoomableY )
    zoomToAllButtonXY->setBounds(zoomInButtonX->getRight(), zoomInButtonY->getBottom(), 
                                 widgetSize, widgetSize);

  // \todo we also need to make the buttons visible/invisible - otherwise they will remain where
  // they are but will still be visible, when zooming is turned off - but maybe do this in
  // setZoomFactorPerClick
}

//-------------------------------------------------------------------------------------------------
// internal functions:

void PanelScroller::updateScrollbars()
{
  if( thePanel == NULL ) 
    return;

  double v1, v2, r;

  v1 = thePanel->getCurrentRangeMinX();
  v2 = thePanel->getCurrentRangeMaxX();
  v1 = transformToScrollBarCoordinateX(v1);
  v2 = transformToScrollBarCoordinateX(v2);
  r  = v2-v1;
  scrollBarX->setCurrentRange(v1, r);
  scrollBarX->setSingleStepSize(0.1*r);

  v1 = thePanel->getCurrentRangeMinY();
  v2 = thePanel->getCurrentRangeMaxY();
  v1 = transformToScrollBarCoordinateY(v1);
  v2 = transformToScrollBarCoordinateY(v2);
  r  = v2-v1;
  scrollBarY->setCurrentRange(v1, r);
  scrollBarY->setSingleStepSize(0.1*r);
}

double PanelScroller::transformToScrollBarCoordinateX(double x) const
{
  if( thePanel == NULL ) 
    return 0.0;
  return mapLinearToLinear(x, thePanel->getMaximumRangeMinX(), 
    thePanel->getMaximumRangeMaxX(), 0.0, 1.0);
}
double PanelScroller::transformToScrollBarCoordinateY(double y) const
{
  if( thePanel == NULL ) 
    return 0.0;
  return mapLinearToLinear(y, thePanel->getMaximumRangeMinY(), 
    thePanel->getMaximumRangeMaxY(), 0.0, 1.0);
}

double PanelScroller::transformFromScrollBarCoordinateX(double x) const
{
  if( thePanel == NULL ) 
    return 0.0;
  return mapLinearToLinear(x, 0.0, 1.0, thePanel->getMaximumRangeMinX(), 
    thePanel->getMaximumRangeMaxX());
}
double PanelScroller::transformFromScrollBarCoordinateY(double y) const
{
  if( thePanel == NULL ) 
    return 0.0;
  return mapLinearToLinear(y, 0.0, 1.0, thePanel->getMaximumRangeMinY(), 
    thePanel->getMaximumRangeMaxY());
}










void ArrangementScroller::assignPanel(Panel* newPanelToAssign)
{
  theArrangementPanel = dynamic_cast<MixsonicArrangementPanel*>(newPanelToAssign);
  jassert(theArrangementPanel);
  PanelScroller::assignPanel(newPanelToAssign);
}

void ArrangementScroller::alignWidgetsToPanel()
{
  PanelScroller::alignWidgetsToPanel();

  // adapt horizontal scrollbar, so it doesn't extend leftward beyond the track-bodies:
  int x = MixsonicTrackControlComponent::getDesiredWidth();
  int w = scrollBarX->getWidth() - x;
  scrollBarX->setBounds(x, scrollBarX->getY(), w, scrollBarX->getHeight());

  // adapt vertical scrollbar, so it doesn't extend upward beyond the tracks:
  int y = theArrangementPanel->getTrackAreaY();
  int h = scrollBarY->getHeight() - y;
  scrollBarY->setBounds(scrollBarY->getX(), y, scrollBarY->getWidth(), h);
}

void ArrangementScroller::zoomY(double zoomFactor, double relativeCenter)
{
  int minTrackHeight = TrackComponent::getMinimumHeight();
  int maxTrackHeight = 3*minTrackHeight;
  int trackHeight    = theArrangementPanel->getTrackHeight();
  int increment      = 15;

  if( zoomFactor > 1.0 )
    trackHeight = jmin(maxTrackHeight, trackHeight+increment);
  else if( zoomFactor < 1.0 )
    trackHeight = jmax(minTrackHeight, trackHeight-increment);
   
  theArrangementPanel->setTrackHeight(trackHeight);
  updateScrollbars();
}

void ArrangementScroller::zoomToAllY()
{
  theArrangementPanel->setTrackHeight(TrackComponent::getMinimumHeight());
  updateScrollbars();
}

void ArrangementScroller::mouseWheelMove(const MouseEvent& e, 
                                         float wheelIncrementX, float wheelIncrementY)
{
  MouseEvent e2 = e.getEventRelativeTo(this);
  int xOffset = MixsonicTrackControlComponent::getDesiredWidth(); // later: getTrackBodyOffset
  int nw      = thePanel->getWidth() - xOffset;  // navigable width
  double relativeX = (double) (e2.x-xOffset) / (double) nw;
  relativeX = jlimit(0.01, 0.99, relativeX);

  if( wheelIncrementY >= 0.f )
    zoomX(zoomFactorPerClickX, relativeX);
  else if( wheelIncrementY < 0.f )
    zoomX(1.f/zoomFactorPerClickX, relativeX);

  updateScrollbars();
}
/*
void ArrangementScroller::paintOverChildren(Graphics &g)
{
  // just for development:
  Colour c((uint8)255, (uint8)0, (uint8)0, (uint8)100);
  g.fillAll(c);
}
*/
