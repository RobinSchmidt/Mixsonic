#ifndef ActivationObserver_h
#define ActivationObserver_h

#include "../../libs/JuceIncludes.h"

class Activatable;

/** Baseclass for objects that must keep track of some other object that may be activated and
deactivated, like, for example, a window. */

class ActivationObserver
{
public:
  virtual void activationStatusChanged(Activatable *activatable, bool isActive) = 0;
};

/** Baseclass for objects whose activation status has to be observed by other objects. */

class Activatable
{

public:

  //~Activatable();

  void registerActivationObserver(ActivationObserver *observer);
  void deregisterActivationObserver(ActivationObserver *observer);

  void sendActivationStatusChangeMessage(bool isActive);

protected:

  Array<ActivationObserver*> activationObservers;

};

#endif
