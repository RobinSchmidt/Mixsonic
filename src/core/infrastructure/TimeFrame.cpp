#include "TimeFrame.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction and static member initialization:

TimeFrame::TimeFrame() 
{
  minTimeInBeats     = 0.0;
  maxTimeInBeats     = 180.0;
  currentTimeInBeats = 0.0;
}

TimeFrame::~TimeFrame()
{

}

//-------------------------------------------------------------------------------------------------
// setup:
/*
void TimeFrame::setMinTime(double newMinTime)
{
  setMinTimeInSeconds(convertSelectedTimeUnitToSeconds(newMinTime));
}

void TimeFrame::setMaxTime(double newMaxTime)
{
  setMaxTimeInSeconds(convertSelectedTimeUnitToSeconds(newMaxTime));
}

void TimeFrame::setCurrentTime(double newCurrentTime)
{
  setCurrentTimeInSeconds(convertSelectedTimeUnitToSeconds(newCurrentTime));
}
*/

void TimeFrame::setMinTimeInBeats(double newMinTimeInBeats)
{
  jassert( newMinTimeInBeats < maxTimeInBeats );
  if( newMinTimeInBeats <= maxTimeInBeats-0.001 )
    minTimeInBeats = newMinTimeInBeats;
}

void TimeFrame::setMaxTimeInBeats(double newMaxTimeInBeats)
{
  jassert( newMaxTimeInBeats > minTimeInBeats );
  if( newMaxTimeInBeats >= minTimeInBeats+0.001 )
    maxTimeInBeats = newMaxTimeInBeats;
}

void TimeFrame::setCurrentTimeInBeats(double newCurrentTimeInBeats)
{  
  currentTimeInBeats = newCurrentTimeInBeats;
}

void TimeFrame::setTimeRangeInBeats(double newMinTime, double newMaxTime)
{
  jassert(newMaxTime > newMinTime);
  if( newMaxTime > newMinTime )
  {
    minTimeInBeats = newMinTime;
    maxTimeInBeats = newMaxTime;
  }
}

void TimeFrame::setMinTimeInSeconds(double newMinTimeInSeconds)
{
  setMinTimeInBeats( secondsToBeats(newMinTimeInSeconds) );
}

void TimeFrame::setMaxTimeInSeconds(double newMaxTimeInSeconds)
{
  setMaxTimeInBeats( secondsToBeats(newMaxTimeInSeconds) );
}

void TimeFrame::setCurrentTimeInSeconds(double newCurrentTimeInSeconds)
{  
  setCurrentTimeInBeats( secondsToBeats(newCurrentTimeInSeconds) );
}

void TimeFrame::setTimeRangeInSeconds(double newMinTime, double newMaxTime)
{
  setTimeRangeInBeats(secondsToBeats(newMinTime), secondsToBeats(newMaxTime));
}

void TimeFrame::setCurrentTimeInSamples(double newCurrentTimeInSamples)
{  
  double newCurrentTimeInSeconds = newCurrentTimeInSamples / systemSampleRate;
  setCurrentTimeInBeats( secondsToBeats(newCurrentTimeInSeconds) );
}

//-------------------------------------------------------------------------------------------------
// inquiry:

/*
double TimeFrame::getMinTime() const
{
  return convertSecondsToSelectedTimeUnit(minTimeInSeconds);
}

double TimeFrame::getMaxTime() const
{
  return convertSecondsToSelectedTimeUnit(maxTimeInSeconds);
}

double TimeFrame::getLength() const
{
  return getMaxTime()-getMinTime();
}

double TimeFrame::getCurrentTime() const
{
  return convertSecondsToSelectedTimeUnit(currentTimeInSeconds);
}
*/

double TimeFrame::getMinTimeInBeats() const
{
  return minTimeInBeats;
}

double TimeFrame::getMaxTimeInBeats() const
{
  return maxTimeInBeats;
}

double TimeFrame::getLengthInBeats() const
{
  return maxTimeInBeats-minTimeInBeats;
}

double TimeFrame::getCurrentTimeInBeats() const
{
  return currentTimeInBeats;
}

double TimeFrame::getMinTimeInSeconds() const
{
  return beatsToSeconds(minTimeInBeats);
}

double TimeFrame::getMaxTimeInSeconds() const
{
  return beatsToSeconds(maxTimeInBeats);
}

double TimeFrame::getLengthInSeconds() const
{
  return beatsToSeconds(maxTimeInBeats-minTimeInBeats);
}

double TimeFrame::getCurrentTimeInSeconds() const
{
  return beatsToSeconds(currentTimeInBeats);
}






