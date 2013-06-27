#include "GlobalFunctions.h"

MixsonicGlobals *mixsonicGlobals = nullptr;   
MixsonicGlobals::MixsonicGlobals()
{
  if( LOGGING_ENABLED )
    logMessage(String::empty); // as separator between runs of the app
}  
MixsonicGlobals::~MixsonicGlobals()
{

}
void MixsonicGlobals::logMessage(const String& message)
{
  if( LOGGING_ENABLED )
  {
    String logString = Time::getCurrentTime().toString(true, true, true, true) 
      + String(":   ") + message + String("\n"); 
    File logFile(File(getApplicationDirectoryAsString() + String("/MixsonicLog.txt")));
    if( !logFile.existsAsFile() )
      logFile.create();
    logFile.appendText(logString);
  }
}


double amp2dB(double amp)
{
  return 8.6858896380650365530225783783321 * log(amp);
  //return 20*log10(amp); // naive version
}

double beatsToSeconds(double beat, double bpm)
{
  return (60.0/bpm)*beat;
}

/*
String createAudioFileInfoString(File fileToCreateStringFrom)
{
  AudioFileInfo info(fileToCreateStringFrom);

  String channelString;
  if( info.numChannels == 1 )
    channelString = String("mono");
  else if( info.numChannels == 2 )
    channelString = String("stereo");
  else
    channelString = String(info.numChannels) + String(" channels");

  String infoString = String(info.sampleRate) + String(" Hz, ") 
    + String(info.bitDepth) + String(" Bit, ") 
    + String(info.numSamples) + String(" frames, ")
    + channelString;

  return infoString;
}
*/

double dB2amp(double dB)
{
  return exp(dB * 0.11512925464970228420089957273422);
  //return pow(10.0, (0.05*dB)); // naive, inefficient version
}

void fitLineToRectangle(double &x1, double &y1, double &x2, double &y2, 
                        double xMin, double yMin, double xMax, double yMax)
{
  // catch some special cases:
  if( x1 == x2 ) // vertical line, infinite slope
  {
    y1 = yMin;
    y2 = yMax;
    return;
  }
  if( y1 == y2 ) // horizontal line, zero slope
  {
    x1 = xMin;
    x2 = xMax;
    return;
  }

  // calculate slope 'a' and constant term 'b' for the line equation y = a*x + b:
  double a = (y2-y1)/(x2-x1);
  double b = y1-a*x1;

  // calculate x- and y-values at the rectangle's boundaries:
  double yAtMinX = a*xMin+b;
  double yAtMaxX = a*xMax+b;
  double xAtMinY = (yMin-b)/a;
  double xAtMaxY = (yMax-b)/a;

  if( yAtMinX > yMin && yAtMinX < yMax )    
    // line intercepts left boundary
  {
    x1 = xMin;    
    y1 = a*xMin+b;
    if( xAtMaxY > xMin && xAtMaxY < xMax )      
      // line intercepts left and top boundary (chops off the top-left corner)
    {
      x2 = (yMax-b)/a;
      y2 = yMax;
    }
    else if( xAtMinY > xMin && xAtMinY < xMax ) 
      // line intercepts left and bottom boundary (chops off the bottom-left corner)
    {
      x2 = (yMin-b)/a;
      y2 = yMin;
    }
    else 
      // line intercepts right boundary (divides the rectangle into top and bottom)
    {
      x2 = xMax;
      y2 = a*xMax+b;
    }
  }
  else if( yAtMaxX > yMin && yAtMaxX < yMax )
    // line intercepts right boundary
  {
    x2 = xMax;
    y2 = a*xMax+b;
    if( xAtMaxY > xMin && xAtMaxY < xMax )
      // line intercepts right and top boundary (chops off top right corner)
    {
      x1 = (yMax-b)/a;
      y1 = yMax;
    }
    else if( xAtMinY > xMin && xAtMinY < xMax )
      // line intercepts right and bottom boundary (chops off bottom right corner)
    {
      x1 = (yMin-b)/a;
      y1 = yMin;
    }
  }
  else if( xAtMaxY > xMin && xAtMaxY < xMax && xAtMinY > xMin && xAtMinY < xMax )
    // line intercepts top and bottom boundary (divides the rectangle into left and right)
  {
    x1 = (yMin-b)/a;
    y1 = yMin;
    x2 = (yMax-b)/a;
    y2 = yMax;
  }

}

File getApplicationDirectory()
{
  File   thisExeAsFile         = File::getSpecialLocation(File::currentApplicationFile);
  File   thisDirectoryAsFile   = thisExeAsFile.getParentDirectory();
	return thisDirectoryAsFile;
}

String getApplicationDirectoryAsString()
{
  return getApplicationDirectory().getFullPathName();
}

File getAudioFileFromPath(const String& path)
{
  File file = File(path);
  if( !file.existsAsFile() )
    return File::nonexistent;

  // the file exists - now check whether it is a valid audio file:
  AudioFileInfo info(file);
  if( !info.isValidAudioFile )
    return File::nonexistent;

  return file;
}

bool hasDirectoryFiles(const File& directoryToCheck)
{
	if( !directoryToCheck.exists() )
		return false;
	
  // when the passed file is not a directory, we consider it as having itself as file:
  if( !directoryToCheck.isDirectory() )
    return true; 

  Array<File> foundFiles; 
  directoryToCheck.findChildFiles(foundFiles, File::findFiles, true);
  if( foundFiles.size() > 0 )
    return true;
  else
    return false;
  // Note: we can't use File::getNumberOfChildFiles here because it doesn't scan recursively
}

AudioPluginInstance* getVSTPluginInstanceFromFile(const File& pluginFile)
{
  KnownPluginList pluginList;
  OwnedArray<PluginDescription> descriptionArray;
  VSTPluginFormat vstFormat;
  bool success = pluginList.scanAndAddFile(pluginFile.getFullPathName(), true, descriptionArray, 
                                           vstFormat);
  PluginDescription* description = pluginList.getTypeForFile(pluginFile.getFullPathName());

  if( description == nullptr )
    return nullptr;
  else
    return vstFormat.createInstanceFromDescription(*description);

  // \todo generalize this function to other formats
}

bool isCloseTo(double x, double targetValue, double tolerance)
{
  if( fabs(x-targetValue) <= tolerance )
    return true;
  else
    return false;
}

bool isEven(int x)
{
  if( x%2 == 0 )
    return true;
  else
    return false;
}

bool isOdd(int x)
{
  if( x%2 != 0 )
    return true;
  else
    return false;
}

bool isPowerOfTwo(unsigned int x)
{
  unsigned int currentPower    = 1;
  while( currentPower <= x )
  {
    if( currentPower == x )
      return true;
    currentPower *= 2;
  }
  return false;
}

bool isStringLegalPathName(String stringToTest)
{
  if( File::createLegalPathName(stringToTest) == stringToTest )
    return true;
  else
    return false;
}

XmlElement* loadXmlFromFile(const File& fileToLoadFrom)
{
  if( !fileToLoadFrom.existsAsFile() )
    return NULL;
  XmlDocument xmlDocument(fileToLoadFrom.loadFileAsString());
  return xmlDocument.getDocumentElement(false);
}

double log2(double x)
{
  return ONE_OVER_LN2*log(x);
}

double logB(double x, double b)
{
  return log(x)/log(b);
}

void logMessage(const String& message)
{
  jassert( mixsonicGlobals != nullptr ); // the mixsonicGlobals was not yet created
  mixsonicGlobals->logMessage(message);
}

double mapLinearToLinear(double in, double inMin, double inMax, double outMin, double outMax)
{
  double tmp;

  // map input to the range 0.0...1.0:
  tmp = (in-inMin) / (inMax-inMin);

  // map the tmp-value to the range outMin...outMax:
  tmp *= (outMax-outMin);
  tmp += outMin;

  return tmp;
}

double mapLinearToExponential(double in, double inMin, double inMax, double outMin, double outMax)
{
  double tmp;

  // map input to the range 0.0...1.0:
  tmp = (in-inMin) / (inMax-inMin);

  // map the tmp-value exponentially to the range outMin...outMax:
  //tmp = outMin * exp( tmp*(log(outMax)-log(outMin)) );
  tmp = outMin * exp( tmp*(log(outMax/outMin)) );

  return tmp;
}

double mapExponentialToLinear(double in, double inMin, double inMax, double outMin, double outMax)
{
  double tmp;

  tmp = log(in/inMin) / log(inMax/inMin);

  tmp = outMin + tmp * (outMax-outMin);

  return tmp;
}

int nextDivisibleBy(int number, int divisor)
{
  if( number % divisor == 0 )
    return number;
  else 
    return number + ( divisor - (number % divisor) );
}

void normalizeAudioSampleBuffer(AudioSampleBuffer* buffer, double peakAmplitude)
{
  float channelMin = 0.0; 
  float channelMax = 0.0;
  float globalMin  = 0.0;
  float globalMax  = 0.0;
  int c;
  for(c=0; c<buffer->getNumChannels(); c++)
  {
    buffer->findMinMax(c, 0, buffer->getNumSamples(), channelMin, channelMax);
    if( channelMin < globalMin )
      globalMin = channelMin;
    if( channelMax > globalMax )
      globalMax = channelMax;
  }
  float normalizer = 1.f / jmax(fabs(globalMin), fabs(globalMax));
  for(c=0; c<buffer->getNumChannels(); c++)
    buffer->applyGain(c, 0, buffer->getNumSamples(), normalizer);
}

double round(double x)
{
  if( x-floor(x) >= 0.5 )
    return ceil(x);
  else
    return floor(x);
}

bool saveXmlToFile(const XmlElement& xmlToSave, const File& fileToSaveTo)
{
  String myXmlDoc = xmlToSave.createDocument(String::empty);

  bool success = false;
  if( fileToSaveTo.existsAsFile() )
  {
    success = fileToSaveTo.deleteFile();
    if( success == false )
    {
      showFileCouldNotBeSavedBox(fileToSaveTo.getFileName());
      return false;
    }
  }
  success = fileToSaveTo.create();
  if( success == false )
  {
    showFileCouldNotBeSavedBox(fileToSaveTo.getFileName());
    return false;
  }
  success = fileToSaveTo.appendText(myXmlDoc);
  if( success == false )
  {
    showFileCouldNotBeSavedBox(fileToSaveTo.getFileName());
    return false;
  }

  return true;
}

double secondsToBeats(double timeInSeconds, double bpm)
{
  return timeInSeconds*(bpm/60.0);
}

String secondsToString(double timeInSeconds)
{
  int minutes = (int) floor(timeInSeconds/60.0);
  int seconds = (int) floor(fmod(timeInSeconds, 60.0));

  String minuteString;
  if( minutes >= 10.0 )
    minuteString = String(minutes);
  else
    minuteString = String(0) + String(minutes);

  String secondString;
  if( seconds >= 10.0 )
    secondString = String(seconds);
  else
    secondString = String(0) + String(seconds);

  return minuteString + String(":") + secondString;
}

double sign(double x)
{
  if(x<0)
    return -1.0;
  else if(x>0)
    return 1.0;
  else
    return 0;
}

String valueToString0(double value)
{
  return String( (int) round(value) ) ;
}

String valueToString2(double value)
{
  return String(value, 2);
}

double wholeNotesToSeconds(double noteValue, double bpm)
{
  return (240.0/bpm)*noteValue;
}
