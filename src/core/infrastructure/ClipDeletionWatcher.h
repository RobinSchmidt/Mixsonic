#ifndef ClipDeletionWatcher_h
#define ClipDeletionWatcher_h

#include "Clip.h"

/**

This class is to be used as baseclass for objects that must keep track of deletion of a Clip 
objects such as the ClipComponent class.

\todo generalize this to a general DeletionWatcher with callback 
objectWillBeDeleted(void *object). the 2nd callback clipNameChanged can be removed and Clip
can send out a generic cchnage message, when the name is changed

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