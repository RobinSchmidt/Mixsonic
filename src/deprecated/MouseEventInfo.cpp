#include "MouseEventInfo.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction:

MouseEventInfo::MouseEventInfo() 
//: source(0, true)
{
  x                 = 0;
  y                 = 0;    
  screenX           = 0;
  screenY           = 0;
  mods              = ModifierKeys();
  eventComponent    = NULL;
  originalComponent = NULL;
  eventTime         = Time::getCurrentTime();
  mouseDownPos      = Point<int>(0, 0);
  numberOfClicks    = 0;
}

MouseEventInfo::MouseEventInfo(const MouseEvent& e) 
//: source(e.source)
{
  setupFromMouseEvent(e);
}

MouseEventInfo::~MouseEventInfo()
{

}

//-------------------------------------------------------------------------------------------------
// setup:

void MouseEventInfo::setupFromMouseEvent(const MouseEvent& e)
{
  x                 = e.x;
  y                 = e.y;    
  screenX           = e.getScreenX();
  screenY           = e.getScreenY();
  mods              = e.mods;
  eventComponent    = e.eventComponent;
  originalComponent = e.originalComponent;
  eventTime         = e.eventTime;
  mouseDownPos      = e.getMouseDownPosition();
  numberOfClicks    = e.getNumberOfClicks();
}

void MouseEventInfo::setPosition(const Point<int>& newPosition)
{
  x = newPosition.getX();
  y = newPosition.getY();
}

//-------------------------------------------------------------------------------------------------
// inquiry:

MouseEventInfo MouseEventInfo::getEventInfoRelativeTo(Component* const otherComponent) const
{
  MouseEventInfo e = *this;
  Point<int> p = otherComponent->getLocalPoint(eventComponent, getPosition());
  e.setPosition(p);
  return e;
}

/*
// from juce::MouseEvent:
MouseEvent MouseEvent::getEventRelativeTo (Component* const otherComponent) const noexcept
{
    jassert (otherComponent != nullptr);

    return MouseEvent (source, otherComponent->getLocalPoint (eventComponent, getPosition()),
                       mods, otherComponent, originalComponent, eventTime,
                       otherComponent->getLocalPoint (eventComponent, mouseDownPos),
                       mouseDownTime, numberOfClicks, wasMovedSinceMouseDown != 0);
}
*/

Point<int> MouseEventInfo::getPosition() const
{
  return Point<int>(x, y);
}
 
MouseEvent MouseEventInfo::createMouseEvent() const
{
  MouseInputSource source(0, true); // create a dummy source
  return MouseEvent(source, Point<int>(x, y), mods, eventComponent, originalComponent, eventTime, 
    mouseDownPos, eventTime, numberOfClicks, false);
}



