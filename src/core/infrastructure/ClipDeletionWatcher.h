#ifndef ClipDeletionWatcher_h
#define ClipDeletionWatcher_h

#include "Clip.h"

/**

This class is to be used as baseclass for objects that must keep track of deletion of a Clip 
objects such as the ClipComponent class.

*/

class ClipDeletionWatcher
{  

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  ClipDeletionWatcher();  

  /** Destructor. */
  virtual ~ClipDeletionWatcher();                             

  //-----------------------------------------------------------------------------------------------
  // callbacks:

  /** Implement this callback method to clear your references to the AudioClip that is going to be 
  deleted. */
  virtual void clipIsToBeDeleted(Clip* clipToBeDeleted) = 0;

  /** Implement this callback method to update everything that depends on the clips name (such as
  the caption of the ClipComponent, for example). */
  virtual void clipNameChanged(Clip* clipThatHasChangedItsName) = 0;

  //===============================================================================================
  juce_UseDebuggingNewOperator;

};

#endif  