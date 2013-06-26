#ifndef ComponentGrabber_h
#define ComponentGrabber_h

#include "../../libs/JuceIncludes.h"

// forward declaration of the GrabbableComponent class:
class GrabbableComponent;

/**

This class is the base-class for all classes that can grab some GrabbableComponent.

*/

class ComponentGrabber
{  

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  ComponentGrabber();  

  /** Destructor. */
  virtual ~ComponentGrabber();                             

  //-----------------------------------------------------------------------------------------------
  // callbacks:

  /** This function must be overriden by your subclass in order to actually grab the component. The 
  second parameter is the original MouseEvent inside the Component that wants to be grabbed that 
  gave rise to the grab-request. The boolean return value should be used to indicate whether or not
  you have sucessfully grabbed the component. */
  virtual bool componentRequestsToBeGrabbed(GrabbableComponent* componentToBeGrabbed, 
    MouseEvent grabEvent) = 0;

  //===============================================================================================
  juce_UseDebuggingNewOperator;

};

#endif  