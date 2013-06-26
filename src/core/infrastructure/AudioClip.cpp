#include "AudioClip.h"
//#include "AudioClipDeletionWatcher.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction:

AudioClip::AudioClip(AudioFileBuffer* newBuffer) : AudioFileBufferUser(newBuffer)
{
  ScopedLock pointerLock(audioFileBufferPointerLock);

  initPlaybackSettings();
  if( bufferToUse != 0 )
    setName(bufferToUse->getFileNameWithoutExtension());
}

AudioClip::AudioClip(const AudioClip& other) : AudioFileBufferUser(other.bufferToUse)
{
  ScopedLock pointerLock(audioFileBufferPointerLock);
  name                        = other.name;
  position                    = other.position;
  totalDuration               = other.totalDuration;
  headingInactiveDuration     = other.headingInactiveDuration;
  trailingInactiveDuration    = other.trailingInactiveDuration;
  mute                        = other.mute;
  hostingTrackIndex           = other.hostingTrackIndex;
  instantiationManager        = other.instantiationManager;
  needsResampling             = other.needsResampling;
  activeRegionStartInSamples  = other.activeRegionStartInSamples;
  activeRegionEndInSamples    = other.activeRegionEndInSamples;
  activeRegionLengthInSamples = other.activeRegionLengthInSamples;
  activeRegionOffsetInSamples = other.activeRegionOffsetInSamples;
  fadeInInSeconds             = other.fadeInInSeconds;
  fadeOutInSeconds            = other.fadeOutInSeconds;
  level                       = other.level;
  clipAmp                     = other.clipAmp;
  fadeInInSamples             = other.fadeInInSamples;
  fadeOutInSamples            = other.fadeOutInSamples;
  fadeOutStartInSamples       = other.fadeOutStartInSamples;
  numChannels                 = other.numChannels;
}

//-------------------------------------------------------------------------------------------------
// callbacks:

void AudioClip::filePathChanged(AudioFileBuffer *bufferWithNewPath)
{
  ScopedLock pointerLock(audioFileBufferPointerLock);

  if( bufferToUse != 0 &&  bufferToUse == bufferWithNewPath )
    setName(bufferToUse->getFileNameWithoutExtension());
  else
    jassertfalse;
}
  
void AudioClip::prepareToPlay(double sampleRate)
{
  ScopedLock pointerLock(audioFileBufferPointerLock);
  if( bufferToUse == NULL )
    return;
  
  systemSampleRate            = sampleRate;
  needsResampling             = (systemSampleRate != bufferToUse->getFileSampleRate());
  activeRegionStartInSamples  = (int) floor( getActiveRegionStart(SAMPLES, false) );
  activeRegionEndInSamples    = (int) ceil(  getActiveRegionEnd(  SAMPLES, false) );
  activeRegionOffsetInSamples = (int) floor( getActiveRegionStart(SAMPLES, true ) );
  activeRegionLengthInSamples = activeRegionEndInSamples - activeRegionStartInSamples + 1;
  fadeInInSamples             = (int) (fadeInInSeconds*systemSampleRate);
  fadeOutInSamples            = (int) (fadeOutInSeconds*systemSampleRate);
  fadeOutStartInSamples       = (int) ceil( getActiveRegionEnd(   SAMPLES, true))
                                - fadeOutInSamples + 1; // check this
}
 
void AudioClip::addSignalToAudioBlock(const AudioSourceChannelInfo &bufferToFill, 
                                      const int64 sliceStartInSamples, 
                                      const double amp, const double pan)
{
  if( mute )
    return;

  // make sure that the pointer to the bufferToUse is not modified during this function:
  ScopedLock pointerLock(audioFileBufferPointerLock);

  // check if we have a valid buffer - if so acquire a read lock for the audio-data, otherwise 
  // return:
  if( bufferToUse == NULL )
    return;
  ScopedReadLock dataLock(bufferToUse->audioDataReadWriteLock);

  // call one of the functions that do the actual work - they themselves don't need to acquire the 
  // locks as we already hold them here, by now:
  if( needsResampling == false )
    addSignalToAudioBlockNoResampling(bufferToFill, sliceStartInSamples, amp, pan);
  else
    addSignalToAudioBlockResampling(bufferToFill, sliceStartInSamples, amp, pan);
}

//-------------------------------------------------------------------------------------------------
// setup:

void AudioClip::assignAudioFileBuffer(AudioFileBuffer* newBuffer)
{
  ScopedLock pointerLock(audioFileBufferPointerLock);

  AudioFileBufferUser::assignAudioFileBuffer(newBuffer);
  if( bufferToUse != 0 )
    setName(bufferToUse->getFileNameWithoutExtension());
  prepareToPlay(systemSampleRate);
}

void AudioClip::setLevel(double newLevel)
{
  level   = newLevel;
  clipAmp = dB2amp(level);
}

void AudioClip::setFadeInInSeconds(double newFadeInInSeconds)
{ 
  fadeInInSeconds = jlimit(0.0, getUpperLimitForFadeInInSeconds(),  newFadeInInSeconds); 
  prepareToPlay(systemSampleRate);
}

void AudioClip::setFadeOutInSeconds(double newFadeOutInSeconds)
{
  fadeOutInSeconds = jlimit(0.0, getUpperLimitForFadeOutInSeconds(),  
                            newFadeOutInSeconds); 
  prepareToPlay(systemSampleRate);
}

void AudioClip::setClipPosition(double newPosition, int timeUnit)
{
  Clip::setClipPosition(newPosition, timeUnit);
  prepareToPlay(systemSampleRate);
}

void AudioClip::setActiveRegionStart(double newStart, int timeUnit, bool inLocalTime)
{
  Clip::setActiveRegionStart(newStart, timeUnit, inLocalTime);
  fadeInInSeconds  = jlimit(0.0, getUpperLimitForFadeInInSeconds(),  fadeInInSeconds); 
  fadeOutInSeconds = jlimit(0.0, getUpperLimitForFadeOutInSeconds(), fadeOutInSeconds); 
  prepareToPlay(systemSampleRate);
}

void AudioClip::setActiveRegionEnd(double newEnd, int timeUnit, bool inLocalTime)
{
  Clip::setActiveRegionEnd(newEnd, timeUnit, inLocalTime);
  fadeOutInSeconds = jlimit(0.0, getUpperLimitForFadeOutInSeconds(), fadeOutInSeconds);
  fadeInInSeconds  = jlimit(0.0, getUpperLimitForFadeInInSeconds(),  fadeInInSeconds); 
  prepareToPlay(systemSampleRate);
}

//-------------------------------------------------------------------------------------------------
// inquiry:

double AudioClip::getLevel() const
{
  return level;
}

double AudioClip::getFadeInInSeconds() const
{
  return fadeInInSeconds;
}

double AudioClip::getFadeOutInSeconds() const
{
  return fadeOutInSeconds;
}

double AudioClip::getUpperLimitForFadeInInSeconds() const
{
  return jmax(0.0, getActiveRegionDuration(SECONDS)-fadeOutInSeconds);
}

double AudioClip::getUpperLimitForFadeOutInSeconds() const
{
  return jmax(0.0, getActiveRegionDuration(SECONDS)-fadeInInSeconds);
}

String AudioClip::getLevelAndFadeString()
{
  return clipSettingsStr1 + String(level, 2) + clipSettingsStr2 + String(fadeInInSeconds, 3)  
    + clipSettingsStr3 + String(fadeOutInSeconds, 3) + clipSettingsStr4;
}

//-------------------------------------------------------------------------------------------------
// others:

void AudioClip::initPlaybackSettings()
{
  fadeInInSeconds          = 0.0;
  fadeOutInSeconds         = 0.0;
  level                    = 0.0;
  clipAmp                  = 1.0;
  //clipPan              = 0.0;

  // these values are set up in prepareToPlay to actually meaningful values - these initializations 
  // are chosen such, that the clip won't be played at all (assuming playback time in samples to be
  // positive):
  activeRegionStartInSamples   = -1;
  activeRegionEndInSamples     = -1;
  activeRegionLengthInSamples  = -1;	
  activeRegionOffsetInSamples  =  0;
  fadeInInSamples              =  0;
  fadeOutInSamples             =  0; 
  fadeOutStartInSamples        = (int) ceil( getActiveRegionEnd(   SAMPLES, true))
                                 - fadeOutInSamples + 1; // check this
  needsResampling              = false;

  ScopedLock pointerLock(audioFileBufferPointerLock);
  if( bufferToUse != NULL )
  {
    if( bufferToUse->getNumSamples() > 0 )
      totalDuration = secondsToBeats(bufferToUse->getLengthInSeconds());	
		numChannels = bufferToUse->getNumChannels();
  }
}

//-------------------------------------------------------------------------------------------------
// state saving and recall:

XmlElement* AudioClip::getStateAsXml(const File& songDirectory) const
{
  XmlElement* xmlState = Clip::getStateAsXml();

  xmlState->setAttribute("type",             "audio");
  xmlState->setAttribute("level",            level);
  //xmlState->setAttribute("pan",              pan);
  xmlState->setAttribute("fadeInInSeconds",  fadeInInSeconds);
  xmlState->setAttribute("fadeOutInSeconds", fadeOutInSeconds);

  // store (and recall) data of the underlying audio-file
  ScopedLock pointerLock(audioFileBufferPointerLock);
  if( bufferToUse != NULL )
  {
    xmlState->setAttribute("audioFileRelativePath", 
      bufferToUse->getUnderlyingFile().getRelativePathFrom(songDirectory));
  }
  else
  {
    jassertfalse; // this clip refers to a NULL-buffer - that should not happen
    xmlState->setAttribute("audioFileRelativePath", "NoFileAssignedToThisClip");
  }

  return xmlState;
}

void AudioClip::setStateFromXml(const juce::XmlElement &xmlState)
{
  Clip::setStateFromXml(xmlState);
  if( xmlState.getStringAttribute("type") == String("audio") )
  {
    level            = xmlState.getDoubleAttribute("level",            0.0);
    clipAmp          = dB2amp(level);
    //clipPan          = xmlState.getDoubleAttribute("pan",              0.0);
    fadeInInSeconds  = xmlState.getDoubleAttribute("fadeInInSeconds",  0.0);
    fadeOutInSeconds = xmlState.getDoubleAttribute("fadeOutInSeconds", 0.0);
  }
  prepareToPlay(systemSampleRate);
}

//-------------------------------------------------------------------------------------------------
// internal functions:

void AudioClip::addSignalToAudioBlockNoResampling(const AudioSourceChannelInfo &bufferToFill, 
                                                  const int64 sliceStartInSamples,                                      
                                                  const double amp, const double pan)
{
  int64 sliceEndInSamples = sliceStartInSamples + bufferToFill.numSamples - 1;

  // return immediately when this clip has no data inside the time-slice in question:
  if(  sliceStartInSamples > activeRegionEndInSamples 
    || sliceEndInSamples   < activeRegionStartInSamples )
  {
    return;
  }

  // we have something to do - aquire the mutex-lock (note that this function assumes to be called 
  // from within a function that already holds a read-lock for the buffer, so we don't acquire it 
  // here):
  if( bufferToUse == NULL )
    return;

  // determine the region where the slice to be filled and this clip overlap and set up the lower
  // and upper indices into which we must write (with respect to the buffer-start):
  int64 readOffset  =  sliceStartInSamples - activeRegionStartInSamples; 
  int64 writeStart  =  0;
  int64 writeEnd    = -1;
  int64 clipLength  =  activeRegionEndInSamples - activeRegionStartInSamples + 1;
  int64 sliceLength =  bufferToFill.numSamples;
  if( sliceStartInSamples <= activeRegionStartInSamples )
  {
    writeStart = -readOffset;
    if( sliceEndInSamples >= activeRegionEndInSamples )
      writeEnd = writeStart + clipLength - 1;
    else
      writeEnd = sliceLength - 1;
  }
  else
  {
    writeStart = 0;
    if( sliceEndInSamples >= activeRegionEndInSamples )
      writeEnd = clipLength - 1 - readOffset;
    else
      writeEnd = writeStart + sliceLength - 1;
  }

  // copy the a chunk of data from the bufferToUse into an appropriate chunk of the output 
  // buffer:
  int64  n;
  float  *writePointerL, *writePointerR, *readPointerL, *readPointerR;
  double ampL = amp * getChannelAmplitudeWithoutRamp(0, pan);  
  double ampR = amp * getChannelAmplitudeWithoutRamp(1, pan);

  if( bufferToFill.buffer->getNumChannels() < 2 )
  {
    jassertfalse;  // this code assumes that the output buffer has (at least) 2 channels
    return;
  }
  writePointerL = bufferToFill.buffer->getSampleData(0, bufferToFill.startSample);
  writePointerR = bufferToFill.buffer->getSampleData(1, bufferToFill.startSample);

  if( bufferToUse->getNumChannels() == 2 )
  {
    readPointerL = bufferToUse->getSampleData(0, 0);
    readPointerR = bufferToUse->getSampleData(1, 0);
  }
  else if( bufferToUse->getNumChannels() == 1 )
  {
    readPointerL = bufferToUse->getSampleData(0, 0);
    readPointerR = bufferToUse->getSampleData(0, 0);
  }
  else
  {
    jassertfalse; // this code assumes that the used audiobuffer has either 1 or 2 channels
    return;
  }

  double ampWithRampL, ampWithRampR;
  int64  nRead;
  for(n=writeStart; n<=writeEnd; n++)
  {
    nRead = activeRegionOffsetInSamples+readOffset+n;

    ampWithRampL = ampL * getRampFactor(nRead);
    ampWithRampR = ampR * getRampFactor(nRead);

    if( nRead < 0 || nRead >= bufferToUse->getNumSamples() )
    {
      //jassertfalse; // trying to read beyond the borders of the buffer?
      continue;
    }

    // read out the bufferToUse, multiply with amplitude and accumulate into the bufferToFill:
    writePointerL[n] += (float) (ampWithRampL*readPointerL[nRead]);
    writePointerR[n] += (float) (ampWithRampR*readPointerR[nRead]);
  }
}

void AudioClip::addSignalToAudioBlockResampling(const AudioSourceChannelInfo &bufferToFill,                                       
                                                const int64 sliceStartInSamples,                                                
                                                const double amp, const double pan)
{
  int64 sliceEndInSamples = sliceStartInSamples + bufferToFill.numSamples - 1;

  // return immediately when this clip has no data inside the time-slice in question:
  if(  sliceStartInSamples > activeRegionEndInSamples 
    || sliceEndInSamples   < activeRegionStartInSamples )
  {
    return;
  }

  // we have something to do - aquire the mutex-lock (note that this function assumes to be called 
  // from within a function that already holds a read-lock for the buffer, so we don't acquire it 
  // here):
  if( bufferToUse == NULL )
    return;

  int64  n;
  float  *writePointerL, *writePointerR;
  double ampL = amp * getChannelAmplitudeWithoutRamp(0, pan);  
  double ampR = amp * getChannelAmplitudeWithoutRamp(1, pan);

  if( bufferToFill.buffer->getNumChannels() < 2 )
  {
    jassertfalse;  // this code assumes that the output buffer has (at least) 2 channels
    return;
  }
  writePointerL = bufferToFill.buffer->getSampleData(0, bufferToFill.startSample);
  writePointerR = bufferToFill.buffer->getSampleData(1, bufferToFill.startSample);

  if( bufferToUse->getNumChannels() == 2 )
  {
    for(n=0; n<bufferToFill.numSamples; n++)
    {
      // calculate the absolute time in seconds:
      double t = (double) (sliceStartInSamples+n) / systemSampleRate;
      writePointerL[n] += (float)(ampL * getSampleAt(t, 0));
      writePointerR[n] += (float)(ampR * getSampleAt(t, 1));
    }
  }
  else if( bufferToUse->getNumChannels() == 1 )
  {
    for(n=0; n<bufferToFill.numSamples; n++)
    {
      // calculate the absolute time in seconds:
      double t = (double) (sliceStartInSamples+n) / systemSampleRate;
      writePointerL[n] += (float)(ampL * getSampleAt(t, 0));
      writePointerR[n] += (float)(ampR * getSampleAt(t, 0));
    }
  }
  else
  {
    jassertfalse; // this code assumes that the output buffer has either 1 or 2 channels
  }
}

float AudioClip::getSampleAt(double globalTimeInSeconds, int channel)
{
  double t = globalTimeInSeconds;
  if( t < getActiveRegionStart(SECONDS, false) || t > getActiveRegionEnd(SECONDS, false) )
    return 0.0;
  else
  {
    // calculate time in seconds relative to this clip's postion and to the start of it's active 
    // region (required for buffer readout position and ramp calculation respectively):
    double tRel  = t - getClipPosition(SECONDS);
    double tRel2 = t - getActiveRegionStart(SECONDS, false);

    // convert relative time in seconds to unit of samples:
    double nRead = tRel*bufferToUse->getFileSampleRate();

    // read out the value from the buffer with interpolation (currently linear - to be replaced 
    // with something better):
    int    nInt  = (int) floor(nRead);
    double nFrac = nRead - (double) nInt;

    if( nInt >= 0 && nInt+1 < bufferToUse->getNumSamples() ) // bounds checking
    {
      // possibly apply ramping (fade-in/-out) to the static amplitude:
      double ampRamp               = 1.0;
      double fadeOutStartInSeconds = getActiveRegionDuration(SECONDS) - fadeOutInSeconds;
      if( tRel2 < fadeInInSeconds )
        ampRamp = (double) tRel2/(double)fadeInInSeconds;
      else if( tRel2 > fadeOutStartInSeconds )
      {
        ampRamp = (1.0 - (double)(tRel2-fadeOutStartInSeconds)/(double)fadeOutInSeconds);
      }
      else
        ampRamp = 1.0;

      jassert( bufferToUse != NULL ); 
      // call AudioClip::getSampleAt only when you have already ensured that the underlying buffer
      // is not NULL (...and use ScopedLock scopedLock(audioFileBufferLock) )

      float* readPointer = bufferToUse->getSampleData(channel, 0);
      float  result = (float) ( ampRamp * ((1.0-nFrac)*readPointer[nInt] + nFrac*readPointer[nInt+1]) );
      return result;
    }
    else
      return 0.0;
  }
}

double AudioClip::getChannelAmplitudeWithoutRamp(int channel, double pan)
{
  jassert( bufferToUse != NULL ); 
  // call AudioClip::getChannelAmplitudeWithoutRamp only when you have already ensured that the 
  // underlying buffer is not NULL (...and use ScopedLock scopedLock(audioFileBufferLock) )

  double panScaled;
  if( bufferToUse->getNumChannels() == 2 )
  {
    // we have a stereo buffer - therefore we use constant power pan:
    panScaled = 0.25*PI*(pan+1.0);
    if( channel == 0)
      return clipAmp*cos(panScaled);
    else // c == 1
      return clipAmp*sin(panScaled); 
  }
  else
  {
    panScaled = 0.5*(pan+1.0);
    if( channel == 0)
      return clipAmp*(1.0-panScaled);
    else // c == 1
      return clipAmp*(panScaled);
  }
}

double AudioClip::getRampFactor(int64 sampleNumber)
{
  if( sampleNumber < fadeInInSamples+activeRegionOffsetInSamples )
    return (double) (sampleNumber-activeRegionOffsetInSamples) / (double) fadeInInSamples;
  else if( sampleNumber > fadeOutStartInSamples )
    return 1.0 - (double) (sampleNumber-fadeOutStartInSamples) / (double) fadeOutInSamples;
  else
    return 1.0;
}
