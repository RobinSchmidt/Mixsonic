#include "GlobalFunctions.h"

/*
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
*/

void alignWithVisibilityConstraintTo(Component *referenceComponent, Component *toBeAlignedComponent)
{
  if( referenceComponent == nullptr || toBeAlignedComponent == nullptr )
    return;
  Point<int> topLeft = referenceComponent->getScreenPosition();
  topLeft.x += referenceComponent->getWidth();
  topLeft = constrainPositionToScreen(topLeft, toBeAlignedComponent->getWidth(), 
                                               toBeAlignedComponent->getHeight());
  toBeAlignedComponent->setTopLeftPosition(topLeft);
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

Point<int> constrainPositionToScreen(Point<int> desiredPosition, int width, int height)
{
  Rectangle<int> workArea = Desktop::getInstance().getMonitorAreaContaining(desiredPosition, true);

  int x  = desiredPosition.x;
  int y  = desiredPosition.y;
  int w  = width;
  int h  = height;
  int xw = workArea.getX();
  int yw = workArea.getY();
  int ww = workArea.getWidth();
  int hw = workArea.getHeight();
  int r  = x  + w;                // desired right
  int rw = xw + ww;               // work area right
  int b  = y  + h;                // desired bottom
  int bw = yw + hw;               // work area bottom

  if( r > rw )
    x -= r-rw;
  if( b > bw )
    y -= b-bw;
  x = jmax(x, xw);
  y = jmax(y, yw);

  return Point<int>(x, y);
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

void drawCrossHatches(Graphics &g, Component *c, float hatchDistanceX, float lineThickness)
{  
  drawUpwardHatches(  g, c, hatchDistanceX, lineThickness);
  drawDownwardHatches(g, c, hatchDistanceX, lineThickness);
}

void drawDownwardHatches(Graphics &g, Component *c, float hatchDistanceX, float lineThickness)
{  
  float y2 = (float) c->getHeight();
  float x1 = -y2;
  float x2 = x1 + (float) c->getHeight();
  while( x1 < (float) c->getWidth() )
  {
    g.drawLine(x1, 0.f, x2, y2, lineThickness);
    x1 += hatchDistanceX;
    x2 += hatchDistanceX;
  }
}

void drawUpwardHatches(Graphics &g, Component *c, float hatchDistanceX, float lineThickness)
{  
  float y1 = (float) c->getHeight();
  float x1 = -y1;
  float x2 = x1 + (float) c->getHeight();
  while( x1 < (float) c->getWidth() )
  {
    g.drawLine(x1, y1, x2, 0.f, lineThickness);
    x1 += hatchDistanceX;
    x2 += hatchDistanceX;
  }
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

Colour getGrayValue(int value)
{
  return Colour::Colour((uint8)value, (uint8)value, (uint8)value, (uint8)255);
}

Colour getMixedColour(const Colour colour1, const Colour colour2, double weight1, double weight2)
{
  float a1 = colour1.getFloatAlpha();
  float r1 = colour1.getFloatRed();
  float g1 = colour1.getFloatGreen();
  float b1 = colour1.getFloatBlue();

  float a2 = colour2.getFloatAlpha();
  float r2 = colour2.getFloatRed();
  float g2 = colour2.getFloatGreen();
  float b2 = colour2.getFloatBlue();

  uint8 a  = (uint8) (255.f * (weight1*a1 + weight2*a2));
  uint8 r  = (uint8) (255.f * (weight1*r1 + weight2*r2));
  uint8 g  = (uint8) (255.f * (weight1*g1 + weight2*g2));
  uint8 b  = (uint8) (255.f * (weight1*b1 + weight2*b2));

  return Colour(r, g, b, a);
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
  return getPluginInstanceFromDescription(description);

  // \todo generalize this function to other formats
}

AudioPluginInstance* getPluginInstanceFromDescription(const PluginDescription* description)
{
  if( description == nullptr )
    return nullptr;
  else
  {
    jassert(description->pluginFormatName == String("VST")); 
      // only VST supported, at the moment. \todo generalize to other formats

    VSTPluginFormat vstFormat;
    return vstFormat.createInstanceFromDescription(*description);
  }
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

bool isInFrontOf(Component *c1, Component *c2) 
{
  ComponentPeer *peer1 = c1->getPeer();
  ComponentPeer *peer2 = c2->getPeer();
  if( peer1 != peer2 )
    return isInFrontOf(peer1, peer2);
  else
  {
    jassertfalse;
      // not yet implemented - it means, that c1 and c2 are both in the same TopLevelComponent
      // - we perhaps have to check the position inside the array of childComponents in the closest
      // common ancestor - or something
  }
  return true; // preliminary
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

File openLoadFileDialog(const String& dialogTitle, const File& initialFileOrDirctory, 
                        const String& filePatternsAllowed)
{
  FileChooser chooser(dialogTitle, initialFileOrDirctory, filePatternsAllowed); 
  if( chooser.browseForFileToOpen() )
    return chooser.getResult();
  else
    return File::nonexistent;
}

File openLoadAudioPluginDialog()
{
  return openLoadFileDialog("Select Plugin", getApplicationDirectory(), "*.dll");
    // later use a user-specified plugin folder stored in the global preferences and 
    // use a system-specific extension - write a function getPluginFileExtensions()
}

String retainOnlyLettersAndNumbers(const String& s)
{
  return s.retainCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890");
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

// platform specific stuff:

#ifdef JUCE_WINDOWS
  #include <Windows.h>
#endif

bool isInFrontOf(ComponentPeer *p1, ComponentPeer *p2)
{
#ifdef JUCE_WINDOWS
  HWND window1 = (HWND) p1->getNativeHandle();
  HWND window2 = (HWND) p2->getNativeHandle();

  /*
  int z1 = 0;
  HWND nextWindow = GetTopWindow(NULL);
  while( nextWindow != NULL )
  {
    if( nextWindow == window1 )
      break;
    z1++;
    nextWindow = GetNextWindow(nextWindow, GW_HWNDNEXT);
  }

  int z2 = 0;
  nextWindow = GetTopWindow(NULL);
  while( nextWindow != NULL )
  {
    if( nextWindow == window2 )
      break;  
    z2++;
    nextWindow = GetNextWindow(nextWindow, GW_HWNDNEXT);
  }

  return z1 < z2;
  */

  
  HWND nextWindow = GetTopWindow(NULL);
  while( nextWindow != NULL )
  {
    if( nextWindow == window1 )
      return true;  // window1 was found first, so p1 is in front of p2
    else if( nextWindow == window2 )
      return false; // window2 was found first, so p2 is in front of p1
    nextWindow = GetNextWindow(nextWindow, GW_HWNDNEXT);
  }
  jassertfalse; // none of the windows was found - should not happen
  return true;
#else
  // todo: provide mac and linux implementations
  return true;
#endif
}
