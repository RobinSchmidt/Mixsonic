#include "TimeUnitConverter.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction and static member initialization:

double TimeUnitConverter::systemSampleRate = 44100.0; 
double TimeUnitConverter::globalTempoInBpm = 120.0;
int    TimeUnitConverter::selectedTimeUnit = BEATS;

TimeUnitConverter::TimeUnitConverter() 
{

}

TimeUnitConverter::~TimeUnitConverter()
{

}

//-------------------------------------------------------------------------------------------------
// setup:

void TimeUnitConverter::setSystemSampleRate(double newSystemSampleRate)
{
  if( newSystemSampleRate > 0.0 )
    systemSampleRate = newSystemSampleRate;
}

void TimeUnitConverter::setGlobalTempoInBpm(double newTempoInBpm)
{
  if( newTempoInBpm > 0.0 )
    globalTempoInBpm = newTempoInBpm;
}

void TimeUnitConverter::setSelectedTimeUnit(int newSelectedTimeUnit)
{
  //jassert( newTimeUnitToUse >= SECONDS && newTimeUnitToUse <= BEATS ); // invalid time-unit index
  if( newSelectedTimeUnit >= SECONDS && newSelectedTimeUnit <= SAMPLES )
    selectedTimeUnit = newSelectedTimeUnit;
}

//-------------------------------------------------------------------------------------------------
// inquiry:

double TimeUnitConverter::getSystemSampleRate()
{
  return systemSampleRate;
}

double TimeUnitConverter::getGlobalTempoInBpm()
{
  return globalTempoInBpm;
}

int TimeUnitConverter::getSelectedTimeUnit()
{
  return selectedTimeUnit;
}

double TimeUnitConverter::convertTimeUnit(const double in, const int inUnit, const int outUnit,                                    
                                          const double sampleRate, const double bpm)
{
  // avoid calculations (and subsequent roundoff-error) when input and output units are equal:
  if( inUnit == outUnit )
    return in;

  // convert into seconds:
  double seconds;
  switch( inUnit )
  {
  case BEATS:   seconds = ::beatsToSeconds(in, bpm); break;
  case SAMPLES: seconds = in/sampleRate;             break;
  default:      seconds = in;
  }

  // convert seconds into the target unit and return the result:
  switch( outUnit )
  {
  case BEATS:   return ::secondsToBeats(seconds, bpm);
  case SAMPLES: return seconds*sampleRate;
  default:      return seconds;
  }
}

double TimeUnitConverter::beatsToSeconds(double timeInBeats)
{
  return ::beatsToSeconds(timeInBeats, globalTempoInBpm);
}

double TimeUnitConverter::secondsToBeats(double timeInSeconds)
{
  return ::secondsToBeats(timeInSeconds, globalTempoInBpm);
}
