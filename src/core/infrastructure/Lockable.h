#ifndef Lockable_h
#define Lockable_h

#include "GlobalFunctions.h"

/** 

Baseclass for objects that need thread-synchronization via a mutex lock. You can either pass
a Critical section object which shall be used, or let the class create it's own Critical section 
object. The former case is useful, if some CriticalSection should be shared among several objects.

*/

class Lockable
{ 

public:

  Lockable(CriticalSection *mutexToUse/* = nullptr*/)
  {
    if( mutexToUse == nullptr )
    {
      mutex     = new CriticalSection;
      ownsMutex = true;
    }
    else
    {
      mutex     = mutexToUse;
      ownsMutex = false;
    }
  }

  virtual ~Lockable()
  {
    if( ownsMutex )
      delete mutex;
  }

  virtual void enterLock() const
  {
    mutex->enter();
  }

  virtual void tryEnterLock() const
  {
    mutex->tryEnter();
  }

  virtual void exitLock() const
  {
    mutex->exit();
  }

  virtual void setMutex(CriticalSection *newMutex)
  {
    mutex = newMutex;
  }

  virtual CriticalSection* getMutex() const
  {
    return mutex;
  }

protected:

  CriticalSection *mutex;

  bool ownsMutex;

  JUCE_LEAK_DETECTOR(Lockable);
};

#endif
