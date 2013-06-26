#ifndef ClipManipulator_h
#define ClipManipulator_h

#include "ClipDeletionWatcher.h"

/**

This class serves as baseclass for classes that access and manipulate objects of class Clip.

maybe the class ClipDeletionWatcher could be absorbed into this....

*/

class ClipManipulator : public ClipDeletionWatcher
{  

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  ClipManipulator(Clip* newClipToEdit); 

  /** Copy Constructor. */
  ClipManipulator(const ClipManipulator &other);

  /** Destructor. */
  virtual ~ClipManipulator();                             

  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Deletes the underlying Clip object (if any) - wrap this into lockUnderlyingClip(), 
  unlockUnderlyingClip() if you really have to use it. */
  virtual void deleteUnderlyingClip();

  //-----------------------------------------------------------------------------------------------
  // inquiry:

  /** Returns a pointer to the underlying AudioClip. WARNING: Be aware that this may be a NULL 
  pointer and that the underlying clip may change or even be deleted in the lifetime of this
  ClipManipulator object - try to avoid this function and if unavoidable, wrap accesses to
  the returned pointer in lockUnderlyingClip(), unlockUnderlyingClip(). */
  virtual Clip* getUnderlyingClip();

  //-----------------------------------------------------------------------------------------------
  // callbacks:

  /** Implements the purely virtual method from ClipDeletionWatcher to clear our reference 
  to the underlying Clip (if that is the one that is going to be deleted, which it will 
  usually be). */
  virtual void clipIsToBeDeleted(Clip* clipToBeDeleted);

  //-----------------------------------------------------------------------------------------------
  // others:

  /** Aquires a mutex-lock for the underlying clip. */
  virtual void lockUnderlyingClip();

  /** Releases the mutex-lock for the underlying clip. */
  virtual void unlockUnderlyingClip();

  //-----------------------------------------------------------------------------------------------
  // delegations to the underlying clip:

  virtual void   setClipPosition(double newPosition, int timeUnit);
  virtual void   setActiveRegionStart(double newStart, int timeUnit, bool inLocalTime);
  virtual void   setActiveRegionEnd(double newEnd, int timeUnit, bool inLocalTime);
  virtual void   addSelectionListener(SelectionListener* listenerToAdd);
  virtual double getClipPosition(int timeUnit) const;
  virtual double getActiveRegionStart(int timeUnit, bool inLocalTime) const;
  virtual double getActiveRegionEnd(int timeUnit, bool inLocalTime) const;
  virtual bool   hasActiveDataBetweenTimeInstants(double timeInstant1, double timeInstant2, 
                   int timeUnit, bool inLocalTime) const;
  virtual bool   isSelected() const;
  virtual int    getHostingTrackIndex() const;
  virtual void   prepareToPlay(double sampleRate);
  virtual void   setInstantiationManager(ClipInstantiationManager* newInstantiationManager);

  //===============================================================================================
  juce_UseDebuggingNewOperator;

protected:

  // the underlying clip object and a mutex-lock for accesses to it:
  Clip             *clipToEdit;
  CriticalSection  clipLock;

};

#endif  