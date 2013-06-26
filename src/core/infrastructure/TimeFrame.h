#ifndef TimeFrame_h
#define TimeFrame_h

#include "TimeUnitConverter.h"

/**

This class serves as baseclass for classes which have some underlying time-frame such as a song
or or particular phrases in a song.

*/

class TimeFrame : public TimeUnitConverter
{  

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  TimeFrame();  

  /** Destructor. */
  virtual ~TimeFrame();                             

  //-----------------------------------------------------------------------------------------------
  // setup:

  /* Sets the minimum time in seconds. */
  //virtual void setMinTimeInSeconds(double newMinTimeInSeconds);

  /** Sets the minimum time in beats. */
  virtual void setMinTimeInBeats(double newMinTimeInBeats);

  /** Sets the maximum time in beats. */
  virtual void setMaxTimeInBeats(double newMaxTimeInBeats);

  /** Sets the time-range in beats. */
  virtual void setTimeRangeInBeats(double newMinTime, double newMaxTime);

  /** Sets the current time in beats. */
  virtual void setCurrentTimeInBeats(double newCurrentTimeInBeats);

  /** Sets the minimum time in seconds. */
  virtual void setMinTimeInSeconds(double newMinTimeInSeconds);

  /** Sets the maximum time in seconds. */
  virtual void setMaxTimeInSeconds(double newMaxTimeInSeconds);

  /** Sets the time-range in seconds. */
  virtual void setTimeRangeInSeconds(double newMinTime, double newMaxTime);

  /** Sets the current time in seconds. */
  virtual void setCurrentTimeInSeconds(double newCurrentTimeInSeconds);

  /** Sets the current time in samples. */
  virtual void setCurrentTimeInSamples(double newCurrentTimeInSamples);

  //-----------------------------------------------------------------------------------------------
  // inquiry:

  /** Returns the minimum time in beats. */
  virtual double getMinTimeInBeats() const;

  /** Returns the maximum time in beats. */
  virtual double getMaxTimeInBeats() const;

  /** Returns the current time in beats. */
  virtual double getCurrentTimeInBeats() const;

  /** Returns the length of the time-frame in seocnds. */
  virtual double getLengthInBeats() const;

  /** Returns the minimum time in seconds. */
  virtual double getMinTimeInSeconds() const;

  /** Returns the maximum time in seconds. */
  virtual double getMaxTimeInSeconds() const;

  /** Returns the current time in seconds. */
  virtual double getCurrentTimeInSeconds() const;

  /** Returns the length of the time-frame in seocnds. */
  virtual double getLengthInSeconds() const;

  //===============================================================================================
  juce_UseDebuggingNewOperator;

protected:

  double minTimeInBeats, maxTimeInBeats, currentTimeInBeats;

};

#endif  