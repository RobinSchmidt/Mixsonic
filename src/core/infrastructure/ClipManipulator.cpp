#include "ClipManipulator.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction:

ClipManipulator::ClipManipulator(Clip* newClipToEdit)
{ 
  ScopedLock sl(clipLock);
  clipToEdit = newClipToEdit;
  if( clipToEdit != NULL )
    clipToEdit->addClipDeletionWatcher(this);
}

ClipManipulator::ClipManipulator(const ClipManipulator &other)
{
  ScopedLock sl(clipLock);

  // when copying a ClipManipulator, we want to create a new AudioClip for it which has the
  // same data as the AudioClip underneath the other ClipManipulator

  // nnaahh...do this in subclass ClipComponent ...here it would make more sense to refer to
  // the same clip
  
  // we may use the standard copy constructor of Clip to create a new Clip based on the 
  // other one...:
  clipToEdit = new Clip(*other.clipToEdit); 

  // ...but we subscribe ourselves as ClipDeletionWatcher:
  if( clipToEdit != NULL )
    clipToEdit->addClipDeletionWatcher(this);
}

ClipManipulator::~ClipManipulator()
{
  ScopedLock sl(clipLock);
  if( clipToEdit != NULL )
  {
    clipToEdit->removeClipDeletionWatcher(this);
    clipToEdit = NULL;
  }
}

//-------------------------------------------------------------------------------------------------
// setup:

void ClipManipulator::deleteUnderlyingClip()
{
  ScopedLock sl(clipLock);
  delete clipToEdit;
  clipToEdit = NULL;
}

//-------------------------------------------------------------------------------------------------
// inquiry:

Clip* ClipManipulator::getUnderlyingClip()
{
  ScopedLock sl(clipLock);
  return clipToEdit;
}

//-------------------------------------------------------------------------------------------------
// callbacks:

void ClipManipulator::clipIsToBeDeleted(Clip *clipToBeDeleted)
{
  ScopedLock sl(clipLock);
  if( clipToBeDeleted == clipToEdit && clipToEdit != NULL )
  {
    clipToEdit->removeClipDeletionWatcher(this);
    clipToEdit = NULL;
  }
}

//-------------------------------------------------------------------------------------------------
// others:

void ClipManipulator::lockUnderlyingClip()
{
  clipLock.enter();
}

void ClipManipulator::unlockUnderlyingClip()
{
  clipLock.exit();
}

//-------------------------------------------------------------------------------------------------
// delegations to the underlying clip:

void ClipManipulator::setClipPosition(double newPosition, int timeUnit)
{
  ScopedLock sl(clipLock);
  if( clipToEdit != NULL )
    clipToEdit->setClipPosition(newPosition, timeUnit);
}

void ClipManipulator::setActiveRegionStart(double newStart, int timeUnit, bool inLocalTime)
{
  ScopedLock sl(clipLock);
  if( clipToEdit != NULL )
    clipToEdit->setActiveRegionStart(newStart, timeUnit, inLocalTime);
}

void ClipManipulator::setActiveRegionEnd(double newEnd, int timeUnit, bool inLocalTime)
{
  ScopedLock sl(clipLock);
  if( clipToEdit != NULL )
    clipToEdit->setActiveRegionEnd(newEnd, timeUnit, inLocalTime);
}

void ClipManipulator::addSelectionListener(SelectionListener* listenerToAdd)
{
  ScopedLock sl(clipLock);
  if( clipToEdit != NULL )
    clipToEdit->addSelectionListener(listenerToAdd);
}

double ClipManipulator::getClipPosition(int timeUnit) const
{
  ScopedLock sl(clipLock);
  if( clipToEdit == NULL )
    return 0.0;
  return clipToEdit->getClipPosition(timeUnit);
}

double ClipManipulator::getActiveRegionStart(int timeUnit, bool inLocalTime) const
{
  ScopedLock sl(clipLock);
  if( clipToEdit == NULL )
    return 0.0;
  return clipToEdit->getActiveRegionStart(timeUnit, inLocalTime);
}

double ClipManipulator::getActiveRegionEnd(int timeUnit, bool inLocalTime) const
{
  ScopedLock sl(clipLock);
  if( clipToEdit == NULL )
    return 0.0;
  return clipToEdit->getActiveRegionEnd(timeUnit, inLocalTime);
}

bool ClipManipulator::hasActiveDataBetweenTimeInstants(double timeInstant1, double timeInstant2, 
                                                          int timeUnit, bool inLocalTime) const
{
  ScopedLock sl(clipLock);
  if( clipToEdit == NULL )
    return false;
  return clipToEdit->hasActiveDataBetweenTimeInstants(
    timeInstant1, timeInstant2, timeUnit, inLocalTime);
}

bool ClipManipulator::isSelected() const
{
  ScopedLock sl(clipLock);
  if( clipToEdit == NULL )
    return false;
  return clipToEdit->isSelected();
}

int ClipManipulator::getHostingTrackIndex() const
{
  ScopedLock sl(clipLock);
  if( clipToEdit == NULL )
    return -1;
  return clipToEdit->getHostingTrackIndex();
}

void ClipManipulator::prepareToPlay(double sampleRate)
{
  ScopedLock sl(clipLock);
  if( clipToEdit != NULL )
    clipToEdit->prepareToPlay(sampleRate);
}

void ClipManipulator::setInstantiationManager(ClipInstantiationManager* newInstantiationManager)
{
  ScopedLock sl(clipLock);
  if( clipToEdit != NULL )
    clipToEdit->setInstantiationManager(newInstantiationManager);
}