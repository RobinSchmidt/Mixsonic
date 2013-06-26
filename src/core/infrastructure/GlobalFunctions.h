#ifndef GlobalFunctions_h
#define GlobalFunctions_h

#include "../audio/AudioFileInfo.h"

/** This file contains a bunch of useful macros and functions for global use. */

// some mathematical contants:
#define PI 3.1415926535897932384626433832795
#define ONE_OVER_LN2 1.4426950408889634073599246810019

#define MIN_CLIPWIDTH 2

// colors - move into a a colorscheme class:
/*
static Colour backgroundColor          = Colour::Colour((uint8) 64, (uint8) 64, (uint8) 64, (uint8)255);
static Colour highlightBackgroundColor = Colour::Colour((uint8) 96, (uint8) 96, (uint8) 96, (uint8)255);
static Colour outlineColor             = Colour::Colour((uint8)192, (uint8)192, (uint8)192, (uint8)255);
static Colour highlightOutlineColor    = Colour::Colour((uint8)224, (uint8)224, (uint8)224, (uint8)255);
static Colour textColor                = Colour::Colour((uint8)192, (uint8)192, (uint8)192, (uint8)255);
static Colour highlightTextColor       = Colour::Colour((uint8)224, (uint8)224, (uint8)224, (uint8)255);
*/

static Colour widgetBackgroundColor    = Colour::Colour((uint8)  0, (uint8)  0, (uint8)  0, (uint8)255);
static Colour backgroundColor          = Colour::Colour((uint8) 64, (uint8) 64, (uint8) 64, (uint8)255);
static Colour highlightBackgroundColor = Colour::Colour((uint8) 96, (uint8) 96, (uint8) 96, (uint8)255);
static Colour outlineColor             = Colour::Colour((uint8)192, (uint8)192, (uint8)192, (uint8)255);
static Colour highlightOutlineColor    = Colour::Colour((uint8)255, (uint8)255, (uint8)255, (uint8)255);
static Colour textColor                = Colour::Colour((uint8)192, (uint8)192, (uint8)192, (uint8)255);
static Colour highlightTextColor       = Colour::Colour((uint8)255, (uint8)255, (uint8)255, (uint8)255);


/** A class to hold objects that should be accessible application wide. A pointer to an object of 
this class "mixsonicGlobals" is declared here and will be assigned to an object which will be 
created in Mixsonic::initialise and deleted in Mixsonic::shutdown, so it will exist for the whole 
lifetime of the application and be accessible from everywhere. */
class MixsonicGlobals
{
public:
  MixsonicGlobals();
  ~MixsonicGlobals();
  void logMessage(const String& message);
  //FileLogger *logger;
  // maybe define the colors here, too
};
extern MixsonicGlobals *mixsonicGlobals; 
#define LOGGING_ENABLED 0 // set this to 1, if you want to write log-messages


/** Converts a raw amplitude value/factor to a value in decibels. */
double amp2dB(double amp);

/** Returns the index of the maximum value in an array where the array should be of 
length numValues. */
template <class T>
int arrayMaxIndex(T* theArray, int numValues);

/** Returns the index of the minimum value in an array where the array should be of 
length numValues. */
template <class T>
int arrayMinIndex(T* theArray, int numValues);

/** Converts a time-stamp given in beats into seconds acording to a tempo measured in beats per 
minute (bpm). */
double beatsToSeconds(double beat, double bpm);

/** Creates a string with informations about an audiofile that was passed. */
//String createAudioFileInfoString(File fileToCreateStringFrom);

/** Converts a value in decibels to a raw amplitude value/factor. */
double dB2amp(double x);

/** Prolongs and/or shortens the line through x1,y1 and x2,y2 such that it fits into the rectangle
bounded by xMin, yMin, and xMax yMax. */
void fitLineToRectangle(double &x1, double &y1, double &x2, double &y2, 
                        double xMin, double yMin, double xMax, double yMax);

/** Returns the directory of the current application (or .dll) as juce::File. */ 
File getApplicationDirectory();

/** Returns the directory of the current application (or .dll) as String. */ 
String getApplicationDirectoryAsString();

/** Returns a file object if the file with the path given by 'path' exists and is a valid 
audio file, otherwise it returns File::nonexistent. */
File getAudioFileFromPath(const String& path);

/** Checks whether a directory or any of its subdirectories (if any) has files in it and returns 
true, if so and false otherwise. */
bool hasDirectoryFiles(const File& directoryToCheck);

/** Checks, if x is close to some target-value within some tolerance. */
bool isCloseTo(double x, double targetValue, double tolerance);

/** Checks, if x is even. */
bool isEven(int x);

/** Checks, if x is odd. */
bool isOdd(int x);

/** Checks, if x is a power of 2. */
bool isPowerOfTwo(unsigned int x);

/** Checks whether or not a given String is a legal path name. */
bool isStringLegalPathName(String stringToTest);

/** Tries to create an XmlElement from some xml-file and returns a pointer to it (or NULL if the
creation was unsuccessful). If it's non-NULL, the caller is responsible for deleting the returned
XmlElement. */
XmlElement* loadXmlFromFile(const File& fileToLoadFrom);

/** Calculates the logarithm to base 2. */
double log2(double x);

/** Calculates logarithm to an arbitrary base b. */
double logB(double x, double b);

/** Writes a message into our log-file. */
void logMessage(const String& message);

/** Converts a value between inMin and inMax into a value between outMin and outMax where the 
mapping is linear for the input and the output. An example: 
y = mapLinearToLinear(x, 0.0, 1.0, -96.0, 24.0) will map the input x assumed to lie inside 
0.0...1.0 to the range between -96.0...24.0. This function is useful to convert between parameter
representations between 0.0...1.0 and the clear-text parameters. */
double mapLinearToLinear(double in, double inMin, double inMax, double outMin, double outMax);

/** Converts a value between inMin and inMax into a value between outMin and outMax where the 
mapping of the output is exponential. an example: y = mapLinearToLinear(x, 0.0, 1.0, 20.0, 20000.0) 
will map the input x assumed to lie inside 0.0...1.0 to the range between 20.0...20000.0 where 
equal differences in the input lead to equal factors in the output. Make sure that the outMin 
value is greater than zero! */
double mapLinearToExponential(double in, double inMin, double inMax, double outMin, double outMax);

/** The Inverse of "mapLinearToExponential" */
double mapExponentialToLinear(double in, double inMin, double inMax, double outMin, double outMax);

/** Returns the next greater number that is divisible by the divisor - if the 'number' itself is 
divisible by 'divisor', the numbet itself will be returned */
int nextDivisibleBy(int number, int divisor);

/** Returns a power of two which is greater than or equal to the input argument. */ 
template <class T>
T nextPowerOfTwo(T x);

/** Normalizes an AudioSampleBuffer such that the peak amplitude has the given value. */
void normalizeAudioSampleBuffer(AudioSampleBuffer* buffer, double peakAmplitude);

/** Returns the nearest integer (as double). */
double round(double x);

/** Tries to write the contents of an XmlElement into a file and reports about success. */
bool saveXmlToFile(const XmlElement& xmlToSave, const File& fileToSaveTo);

/** Converts a time value in seconds into a time value measured in beats. */
double secondsToBeats(double timeInSeconds, double bpm);

/** Converts a time value in seconds into a formatted String showing minutes and seconds as 
mm:ss. */
String secondsToString(double timeInSeconds);

/** Returns the sign of x as double. */
double sign(double x);
  
/** Swaps two objects of class T. */  
template <class T>
void swap(T &in1, T &in2);

/** Converts a value into a juce-string without decimal digits after the point.  */
String valueToString0(double value);

/** Converts a value into a juce-string with 2 decimal digits after the point.  */
String valueToString2(double value);

/** Converts a time-stamp given in whole notes into seconds according to a tempo measured in 
beats per minute (bpm). */
double wholeNotesToSeconds(double noteValue, double bpm);

//=================================================================================================
// implementation of the templated functions (they must be in the header file):

template <class T>
int arrayMaxIndex(T* theArray, int numValues)
{
  int    maxIndex = 0;
  double maxValue = theArray[0];
  for(int i=0; i<numValues; i++)
  {
    if( theArray[i] > maxValue )
    {
      maxValue = theArray[i];
      maxIndex = i;
    }
  }
  return maxIndex;
}

template <class T>
int arrayMinIndex(T* theArray, int numValues)
{
  int    minIndex = 0;
  double minValue = theArray[0];
  for(int i=0; i<numValues; i++)
  {
    if( theArray[i] < minValue )
    {
      minValue = theArray[i];
      minIndex = i;
    }
  }
  return minIndex;
}

template <class T>
T nextPowerOfTwo(T x)
{
  T accu = 1;
  while(accu < x)
    accu *= 2;
  return accu;
}

template <class T>
void swap(T &in1, T &in2)
{
  T tmp = in1;
  in1   = in2;
  in2   = tmp;   
}

#endif 