#ifndef MouseEventInfo_h
#define MouseEventInfo_h

#include "../../libs/JuceIncludes.h"

/**

This class can be used to store and later retrieve information about juce::MouseEvent objects.
The members closely parallel the members of juce::MouseEvent. This class is needed as a workaround
when we want to store information about mouse-events for later use, because juce::MouseEvent
doesn't support the copy constructor or assignment operator, so we can't store away such events
directly. It's ugly, but anyway.

\todo: we are currently saving pointers to eventComponent and originalComponent, but when we store 
the event to use it later, these components may have gone out of existence at that later time 
instant (maybe that's the reason why juce doesn't allow copying of mouse-events in the first 
place?). in order to implement getEventInfoRelativeTo, we use the eventComponent but maybe we can 
circumvent that by storing screen coordinates instead of the component pointers.

*/

class MouseEventInfo 
{

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Default Constructor. Creates a MouseEventInfo object and initializes the data with default 
  values. */
  MouseEventInfo();

  /** Constructor. Creates a MouseEventInfo object from an actual juce::MouseEvent, pulling all the
  data from the MouseEvent object. */
  MouseEventInfo(const MouseEvent& e);   

  /** Destructor. */
  virtual ~MouseEventInfo(); 



  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Pulls all the relevant data from the passed MouseEvent object and stores it in our member 
  variables. */
  void setupFromMouseEvent(const MouseEvent& e);

  /** Assigns a new position to the event info */
  void setPosition(const Point<int>& newPosition);

  //-----------------------------------------------------------------------------------------------
  // inquiry:

  /** Creates a version of this event-info that is relative to a different component. The x and y 
  positions of the event that is returned will have been adjusted to be relative to the new 
  component. The component pointer that is passed-in must not be null. */
  MouseEventInfo getEventInfoRelativeTo(Component* const otherComponent) const;

  /** Returns the coordinates, where the event has occurred. */
  Point<int> getPosition() const;

  /** Creates an actual juce::MouseEvent object from this MouseEventInfo object. */
  MouseEvent createMouseEvent() const;

  //===============================================================================================
  juce_UseDebuggingNewOperator;

protected:

  int x, y;                      // coordinates with respect to the event component
  int screenX, screenY;          // coordinates with respect to the screen origin;
  ModifierKeys mods;
  Component* eventComponent;
  Component* originalComponent;
  Time eventTime;
  //MouseInputSource& source;      // awkward to init and probably not necessary
  //MouseInputSource source;

  // these are private in juce::MouseEvent:  
  Point<int> mouseDownPos;
  //Time mouseDownTime;            // awkward to init and probably not necessary
  uint8 numberOfClicks; 
  //uint8 wasMovedSinceMouseDown;  // awkward to init and probably not necessary

};

#endif  
