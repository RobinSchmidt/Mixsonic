#ifndef DeletionManagement_h
#define DeletionManagement_h

#include "../../libs/JuceIncludes.h"

class DeletionManager;

/**

Baseclass for objects that need to request their own deletion from some other object. This other 
object should be a subclass of DeletionManager. These two classes work in tandem to provide a 
mechanism by which objects can (indirectly) delete themselves, which is not possible directly.

*/

class DeletionRequester
{  

public:

  /** Constructor. */
  DeletionRequester(DeletionManager *deletionManagerToUse = nullptr);  

  /** Sets our deletion manager, which this object calls when it wants to request its own 
  deletion. */
  virtual void setDeletionManager(DeletionManager *newDeletionManager);

  /** Returns our delettion-manager. */
  virtual DeletionManager* getDeletionManager();

  /** Function to be called, when the object wants to get deleted. It will call a 
  deletionRequest callback on our deletionManager. */
  virtual void requestDeletion();

protected:

  DeletionManager *deletionManager;

  JUCE_LEAK_DETECTOR(DeletionRequester);
};

//=================================================================================================

/**

Baseclass for objects that need handle deletion requests from some object of a subclass of
DeletionRequester.

*/

class DeletionManager
{  

public:

  /** Handles a deletion request from some object that wants to be deleted. The default 
  implementation calls the delete-operator on the passed pointer. You probably want to override 
  this behavior, for example to additionaly nullify any pointers that you may have to the  
  "objectThatWantsToBeDeleted". 

  Remark: It would be nice to pass a reference-to-pointer to this function, so we can implement 
  the nullification directly here. However, that doesn't seem to be possible because we can't pass 
  a this-pointer by a non-const reference (which is what DeletionRequester::requestDeletion does). 
  */
  virtual void handleDeletionRequest(DeletionRequester *objectThatWantsToBeDeleted);

  JUCE_LEAK_DETECTOR(DeletionManager);
};


// \todo create classes DeletionObserver and DeletionObservedSubject in this file, too
// derive ClipDeletionWatcher from DeletionObserver and clip from DeletionObservedSubject

#endif  
