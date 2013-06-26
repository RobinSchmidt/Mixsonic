#ifndef GrabbableComponent_h
#define GrabbableComponent_h

#include "ComponentGrabber.h"

/**

This class is a component that can be grabbed via mouseclicks inside it. This is done by sending 
out a grab-request to an associated ComponentGrabber via the callback function 
componentRequestsToBeGrabbed which the ComponentGrabber subclass must implement.

*/

class GrabbableComponent : virtual public Component
{  

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  GrabbableComponent(const String &componentName = String("GrabbableComponent"));  

  /** Destructor. */
  virtual ~GrabbableComponent();                             

  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Assigns a ComponentGrabber to this GrabbableComponent - this grabber will be called when the 
  component wants to be grabbed via its componentRequestsToBeGrabbed callback. */
  virtual void setComponentGrabber(ComponentGrabber* newGrabber);

  //-----------------------------------------------------------------------------------------------
  // callbacks:

  /** Overrides mouseDown in order to call the grabber's componentRequestsToBeGrabbed callback. */
  virtual void mouseDown(const MouseEvent &e);

  //===============================================================================================
  juce_UseDebuggingNewOperator;

protected:

  ComponentGrabber* grabber;

};

#endif  