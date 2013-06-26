#ifndef ClipInstantiationManager_h
#define ClipInstantiationManager_h

#include "Clip.h"

/**

This class is to be used as baseclass for objects that must manage the instantiation of Clip 
objects. Instantiation management (construction/destruction) is something that can't really be 
handled inside the Clip class, yet it might be desireable for clips to have a means to control 
creation and destruction of new clips. This is what the ClipInstantiationManager class is made for.
It provides a callback system with which a Clip may request operations that involve construction
and/or destruction such as deleting a Clip or splitting a clip into two Clips (the latter involves 
the construction of a new Clip via the copy-constructor and setting up active regions for the old 
and new one appropriately).

*/

class ClipInstantiationManager
{  

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  ClipInstantiationManager();  

  /** Destructor. */
  virtual ~ClipInstantiationManager();                             

  //-----------------------------------------------------------------------------------------------
  // callbacks:

  /** This callback function will be called when a clip wants to be deleted. When you implement 
  this function in your subclass, make sure that you don't just directly delete the Clip object 
  that requests its own deletion, because then, the Clip would indirectly delete itself from its 
  own member function. Instead, move it into an garbage area for later deletion (and do the same 
  for objects which are associated with the Clip (such as a ClipComponent), when this associated 
  object may trigger a sendDeletionRequest in the Clip). */
  virtual void handleDeletionRequest(Clip* clip) = 0;

  /** This callback function will be called when a clip wants splitted into two clips. */
  virtual void handleSplitRequest(Clip* clip, double timeInstantAtWhichToSplit, int timeUnit, 
    bool inLocalTime) = 0;

  //===============================================================================================
  juce_UseDebuggingNewOperator;

};

#endif  