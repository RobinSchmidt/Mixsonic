#ifndef TransportController_h
#define TransportController_h

#include "../libs/JuceIncludes.h"

/** Abstract baseclass for classes that want to receive transport-control callbacks from a 
TransportController.

\todo maybe instead of using the observer-pattern, we could more elegeantly use member-function 
callback objects (less code and Arrangement(Player) could be agnostic of the 
TransportControlObserver class, which then would not need to exist at all).

*/
class TransportControlObserver
{
public:
  virtual void playbackStarted() = 0;
  virtual void playbackStopped() = 0;
  virtual void wasRewindedToStart() = 0;
  virtual void wasSteppedBackwardLargeStep() {};
  virtual void wasSteppedForwardLargeStep() {};
  //virtual void wasSteppedBackwardSmallStep() {};
  //virtual void wasSteppedForwardSmallStep() {};
  //virtual void wasForwardedToEnd() {};
};

/** A class for controlling playback and jumping around in some underlying time-based stream
such as audio or video. */
class TransportController
{

public:

  TransportController();

  void registerObserver(TransportControlObserver *observer);
  void deregisterObserver(TransportControlObserver *observer);

  virtual void startPlayback();
  virtual void stopPlayback();
  virtual void togglePlayback(bool shouldPlay);
  virtual void rewindToStart();
  virtual void largeStepBackward();
  virtual void largeStepForward();
  //virtual void smallStepBackward();
  //virtual void smallStepForward();
  //virtual void forwardToEnd();

  virtual bool isPlaying();

protected:

  Array<TransportControlObserver*> observers;

  bool playing;

  // \todo maybe have more sophisticated controls like a bar which shows and manipulates the 
  // current position inside, maybe also a numeric siplay of current time, etc. - in this case, 
  // we need to store information about the total length and current position here. maybe, we can 
  // then also have functions like jumpTo(double newPosition), etc.
 
  // \todo include loop-toggling, have a (virtual) function that returns a string for numeric 
  // display of the current position


};

// class Arrangement (later ArrangementPlayer) should then be derived from TransportControlListener

#endif