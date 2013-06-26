#include "TransportController.h"

TransportController::TransportController()
{
  playing = false;
}

void TransportController::registerObserver(TransportControlObserver *observer)
{
  observers.addIfNotAlreadyThere(observer);
}

void TransportController::deregisterObserver(TransportControlObserver *observer)
{
  observers.removeValue(observer);
}

void TransportController::startPlayback()
{
  playing = true;
  for(int i = 0; i < observers.size(); i++)
    observers[i]->playbackStarted();
}

void TransportController::stopPlayback()
{
  playing = false;
  for(int i = 0; i < observers.size(); i++)
    observers[i]->playbackStopped();
}
 
void TransportController::togglePlayback(bool shouldPlay)
{
  if( shouldPlay )
    startPlayback();
  else
    stopPlayback();
}

void TransportController::rewindToStart()
{
  for(int i = 0; i < observers.size(); i++)
    observers[i]->wasRewindedToStart();
}

void TransportController::largeStepBackward()
{
  for(int i = 0; i < observers.size(); i++)
    observers[i]->wasSteppedBackwardLargeStep();
}

void TransportController::largeStepForward()
{
  for(int i = 0; i < observers.size(); i++)
    observers[i]->wasSteppedForwardLargeStep();
}
/*
void TransportController::smallStepBackward()
{
  for(int i = 0; i < observers.size(); i++)
    observers[i]->wasSteppedBackwardSmallStep();
}

void TransportController::smallStepForward()
{
  for(int i = 0; i < observers.size(); i++)
    observers[i]->wasSteppedForwardSmallStep();
}
*/
bool TransportController::isPlaying()
{
  return playing;
}
