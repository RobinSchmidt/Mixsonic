#ifndef TimeUnitConverter_h
#define TimeUnitConverter_h

#include "GlobalFunctions.h"

/**

This class serves as base class for all musical objects that in some way need some underlying time
reference. The class will convert betweenn the various time units that are relevant in the context 
of musical applications (beats, seconds and samples) defines a preferred time unit.

*/

class TimeUnitConverter
{  

public:

  /** Enumeration of the time units between which can be converted via convertTimeUnit. */
  enum timeUnits
  {
    SECONDS = 1,
    BEATS,
    SAMPLES
  };

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  TimeUnitConverter();  

  /** Destructor. */
  virtual ~TimeUnitConverter();                             

  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Sets up systems sample-rate - this is needed to convert between time values in seconds, 
  beats, samples, etc. */
  static void setSystemSampleRate(double newSystemSampleRate);

  /** Sets up the song tempo. */
  static void setGlobalTempoInBpm(double newTempoInBpm);

  /** Selects a unit to be used for representing time throughout the application. */
  static void setSelectedTimeUnit(int newSelectedTimeUnit);

  //-----------------------------------------------------------------------------------------------
  // inquiry:

  /** Returns the timeline the sample-rate used by this timeline. */
  static double getSystemSampleRate();

  /** Returns up the song tempo used by the TimeLine. */
  static double getGlobalTempoInBpm();

  /** Returns the selected time unit which is used to represent time throughout the application. */
  static int getSelectedTimeUnit();

  //-----------------------------------------------------------------------------------------------
  // time conversion:

  /** Converts between various time units. The input and output units are specified by the second 
  and third argument (@see: enum timeUnits), also sampleRate and tempo in bpm must be given. */
  static double convertTimeUnit(const double in, const int inUnit, const int outUnit, 
    const double sampleRate, const double bpm);

  /** Converts a time value given in beats into a value in seconds, based on the chosen tempo. */
  static double beatsToSeconds(double timeInBeats);

  /** Converts a time value given in seconds into a value in beats, based on the chosen tempo. */
  static double secondsToBeats(double timeInSeconds);

  //===============================================================================================
  juce_UseDebuggingNewOperator;

protected:

  static double globalTempoInBpm;
  static double systemSampleRate; // static - should be shared bewteen all TimeUnitConverters
  static int    selectedTimeUnit;

};

#endif  