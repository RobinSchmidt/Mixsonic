#ifndef AudioClip_h
#define AudioClip_h

#include <float.h>
#include "Clip.h"
#include "../audio/AudioFileBufferUser.h"

/**

This class represents an audio clip that can be used in audio sequencer applications. Besides a 
pointer to the actual audio data (in form of a juce::AudioSampleBuffer), it stores additional 
information like the relative path to the source file and parameters which control the playback.

\todo
Maybe factor out a baseclass AccumulatingAudioSource with virtual function addSignalToAudioBlock
and (maybe) prepareToPlay, releaseResources. - could be used as baseclass for Track as well. For
efficiency reasons, we should probably not derive it from juce::AudioSource.

*/

class AudioClip : public Clip, public AudioFileBufferUser
{  

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  AudioClip(AudioFileBuffer* newBuffer = NULL);  

  /** Copy Constructor. Copies all data from the other clip except the array of 
  AudioClipDeletionWatchers. ....do we still neeed this? */
  AudioClip(const AudioClip& other);                       

  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Assigns the AudioFileBuffer object to be used for playback and initializes the 
  playback parameters. */
  virtual void assignAudioFileBuffer(AudioFileBuffer* newBuffer);

  /** Sets the clip's volume level in dB. */
  virtual void setLevel(double newLevel);

  /** Sets the clip's fade-in time in seconds. */
  virtual void setFadeInInSeconds(double newFadeInInSeconds);

  /** Sets the clip's fade-out time in seconds. */
  virtual void setFadeOutInSeconds(double newFadeOutInSeconds);

  /** Overrides the method inherited from Clip in order to call prepareToPlay after calling the 
  baseclass' method. */
  virtual void setClipPosition(double newPosition, int timeUnit);

  /** Overrides the method inherited from Clip in order to call prepareToPlay after calling the 
  baseclass' method. */
  virtual void setActiveRegionStart(double newStart, int timeUnit, bool inLocalTime);

  /** Overrides the method inherited from Clip in order to call prepareToPlay after calling the 
  baseclass' method. */
  virtual void setActiveRegionEnd(double newEnd, int timeUnit, bool inLocalTime);

  //-----------------------------------------------------------------------------------------------
  // inquiry:

  /** Returns the clip's volume level in dB. */
  virtual double getLevel() const;

  /** Returns the clip's fade-in time in seconds. */
  virtual double getFadeInInSeconds() const;

  /** Returns the clip's fade-out time in seconds. */
  virtual double getFadeOutInSeconds() const;

  /** Returns the upper limit for the fade-in ramp in seconds. */
  virtual double getUpperLimitForFadeInInSeconds() const;

  /** Returns the upper limit for the fade-out ramp in seconds. */
  virtual double getUpperLimitForFadeOutInSeconds() const;

  /** Returns a String that informs abouts the level and fade-in/out parameters of this clip. */
  virtual String getLevelAndFadeString();

  //-----------------------------------------------------------------------------------------------
  // callbacks:

  /** Gets called from the underlying AudioFileBuffer when the file was renamed - we update our 
  clip-name here to reflect the new filename. */
  virtual void filePathChanged(AudioFileBuffer *bufferWithNewPath);

  /** Should be called before playing back an AudioClip - it will calculate the exact start- and 
  end-positions of the clip in samples and store them in some dedicated member variables which are
  used by addSignalToAudioBlock. */
  virtual void prepareToPlay(double sampleRate);

  /** Generates the output of this clip and adds it to the buffer, sliceStart represents the 
  global start time of the time slice to fill (measured in samples). The pan parameter will be used
  to position the output signal in the stereo field - if the underlying audio data is itself a 
  stereo signal, a constant power rule will be used, otherwise a cosntant sum rule. */
  //virtual void addSignalToAudioBlock(const AudioSourceChannelInfo &bufferToFill, 
  //  const int sliceStartInSamples, const double amp = 1.0, const double pan = 0.0);  // 1.46
  virtual void addSignalToAudioBlock(const AudioSourceChannelInfo &bufferToFill, 
    const int64 sliceStartInSamples, const double amp = 1.0, const double pan = 0.0);  // 1.46

  //-----------------------------------------------------------------------------------------------
  // state saving and recall:

  /** Returns the state (i.e. the settings of all relevant parameters) in form of an XmlElement. 
  The argument must specify the directory into which the song project-file will be ultimately 
  written - this is needed to store the path of our underlying audio file as relative path with 
  respect to that song-directory. */
  virtual XmlElement* getStateAsXml(const File& songDirectory) const;

  /** Recalls a state (i.e. the settings of all relevant parameters) from an XmlElement. It will
  not handle actual loading of the underlying file - this is supposed to be done by an outlying
  class which contains this AudioClip because we want to store the pool of audio buffers at some
  central location. */
  virtual void setStateFromXml(const XmlElement& xmlState);

  //===============================================================================================
  juce_UseDebuggingNewOperator;

protected:

  /** Returns the value of the fade-in/out ramp at a given sample - will be unity outside the fade
  regions. 
  \todo: inline this function - it's called per sample */
  virtual double getRampFactor(int64 sampleNumber);

  /** Initializes the playback settings for the buffer to default values. */
  virtual void initPlaybackSettings();

  /** Flag to indicate whether we need to resample during playback. */
  bool needsResampling;

  /** The start and end of the clip in unit of samples measured in global time. */
  int activeRegionStartInSamples, activeRegionEndInSamples, activeRegionLengthInSamples, 
    activeRegionOffsetInSamples;

  // these are measured in local time:
  double fadeInInSeconds, fadeOutInSeconds, level, clipAmp; // clipPan;
  int fadeInInSamples, fadeOutInSamples, fadeOutStartInSamples, numChannels;

  // ATTENTION: if you add more member variables here, make sure that you copy their values in the 
  // copy constructor (also in the copy constructors of subclasses, if any).

private:

  /** Adds an appropriate chunk from the bufferToPlay to the bufferToFill - used internally by
  addSignalToAudioBlock when the playback-sampleRate and the intrinsic sample-rate of the 
  bufferToPlay match. This function accesses the bufferToUse WITHOUT acquiring the lock before - 
  call it only inside methods when they already hold the lock. */
  virtual void addSignalToAudioBlockNoResampling(const AudioSourceChannelInfo &bufferToFill, 
    const int64 sliceStartInSamples, const double amp = 1.0, const double pan = 0.0);

  /** Adds an appropriate chunk from the bufferToPlay to the bufferToFill by interpolating the
  bufferToPlay - used internally by addSignalToAudioBlock when the playback-sampleRate and the 
  intrinsic sample-rate of the bufferToPlay do not match. This function accesses the bufferToUse 
  WITHOUT acquiring the lock before - call it only inside methods when they already hold the 
  lock.*/
  virtual void addSignalToAudioBlockResampling(const AudioSourceChannelInfo &bufferToFill, 
    const int64 sliceStartInSamples, const double amp = 1.0, const double pan = 0.0);

  /** Returns an interpolated output sample at some time instant given in seconds (in global 
  time). This function accesses the bufferToUse WITHOUT acquiring the lock before - call it only 
  inside methods when they already hold the lock. */
  virtual float getSampleAt(double globalTimeInSeconds, int channel);

  /** Returns the amplitude fo one of the channel (0=left, 1=right) according to the pan parameter 
  - this wil apply different pan rules, depending on whether the bufferToUse is mono (->linear pan 
  rule) or stereo (constant power pan rule). */
  virtual double getChannelAmplitudeWithoutRamp(int channel, double pan);

};

#endif  