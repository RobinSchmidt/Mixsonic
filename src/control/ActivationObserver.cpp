#include "ActivationObserver.h"



void Activatable::registerActivationObserver(ActivationObserver *observer)
{
  activationObservers.addIfNotAlreadyThere(observer);
}

void Activatable::deregisterActivationObserver(ActivationObserver *observer)
{
  activationObservers.removeValue(observer);
}

void Activatable::sendActivationStatusChangeMessage(bool isActive)
{
  for(int i = 0; i < activationObservers.size(); i++)
    activationObservers[i]->activationStatusChanged(this, isActive);
}
