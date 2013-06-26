#ifndef Track_h
#define Track_h

#include "AudioClip.h"
#include "PluginChain.h"

class Track;

/**

Baseclass for object which must observe a Track object. 

*/

class TrackObserver
{
public:

  /** Will be called, when the solo mode for a track was turned on or off. This will possibly have 
  to affect the solo-state of all other tracks, so it is broadcasted. */
  virtual void trackSoloStateChanged(Track *track, bool isSolo) = 0;

};

//=================================================================================================

/**

This class implements the concept of a Track onto which various Clips can be placed.

*/

class Track
{  

  //friend class TrackComponent; // this component needs direct access to the clips
  friend class MixsonicTrackControlComponent; // needs direct access to the pluginChain
  friend class TrackBodyComponent;    // needs direct access to clips
  friend class Arrangement;           // ditto

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  Track(int newIndex);  

  /** Destructor. */
  virtual ~Track();                             

  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Sets the name of the Track. */
  virtual void setName(const String& newName) { name = newName; }

  /** Sets the volume level of the Track in dB */
  virtual void setVolumeLevel(double newLevel);

  /** Sets the panorama position of the Track (values should be between -1...+1). */
  virtual void setPan(double newPan);

  /** Sets the solo-flag for this Track true or false. If the solo-state is active, our observers
  will get a call to trackWasSetToSolo. */
  virtual void setSolo(bool shouldBeSolo);

  /** Sets the mute-flag for this Track true or false. */
  virtual void setMute(bool shouldBeMuted) { mute = shouldBeMuted; }

  /** Sets the index of this Track. @see getIndex */
  virtual void setIndex(int newIndex);

  /** Registers an observer object that will be called back when certain settings change. */
  virtual void registerObserver(TrackObserver *observer);

  /** Deregisters a previously registered observer. */
  virtual void deregisterObserver(TrackObserver *observer);

  //-----------------------------------------------------------------------------------------------
  // inquiry:

  /** Returns the name of the Track. */
  virtual String getName() const { return name; }

  /** Returns the volume level of the Track in dB */
  virtual double getVolumeLevel() const { return level; };

  /** Returns the panorama position of the Track as value between -1...+1. */
  virtual double getPan() const { return pan; };

  /** Indicates whether or not this Track is in solo mode. */
  virtual bool isSolo() const { return solo; }

  /** Indicates whether or not this Track is muted. */
  virtual bool isMuted() const { return mute; }

  /** Returns the index of this Track. @see setIndex */
  virtual int getIndex() const { return index; }

  /** Returns the number of Clips on this Track */
  virtual int getNumClips() const { return audioClips.size(); }

  /** Returns true, when the clip with the given index is selected, false otherwise (or also when 
  the index is invalid). */
  virtual bool isClipSelected(int clipIndex);

  //-----------------------------------------------------------------------------------------------
  // add/remove/etc. clips:

  /** De-selects all possibly the selected clips. */
  virtual void deSelectAllClips();

  /** Adds a clip to this track. */
  virtual void addClip(AudioClip* clipToAdd);

  /** Removes a Clip from this Track or does nothing, if the Clip is not found on this track - 
  the return value informs about whether or not a clip was actually removed. The second parameter 
  decides if the clip will be only removed from the array or if the underlying object will be also 
  deleted. */
  virtual bool removeClip(AudioClip* clipToRemove, bool deleteObject);

  /** Removes a Clip with a given index from this Track or does nothing, if the index is out of 
  range - the return value informs about whether or not a clip was actually removed. The second 
  parameter decides if the clip will be only removed from the array or if the underlying object 
  will be also deleted. */
  virtual bool removeClip(int index, bool deleteObject);

  /** Removes all clips from the Track and optionally deletes the objects. */
  virtual void removeAllClips(bool deleteObjects);

  /** Removes all AudioClips from the Track that use some particular underlying AudioSampleBuffer 
  - this should be used when the respective buffer is going to be deleted. */
  virtual void removeAllClipsUsingBuffer(AudioFileBuffer* buffer, bool deleteObjects);

  /** Creates a copy of one of the clips on this Track and returns a pointer to it. The caller is 
  responsible for the further lifetime of this copy. A NULL pointer will be returned when the 
  index is out of range.  */
  virtual AudioClip* getClipCopy(int index);

  /** Deletes the selected clips from the Track. */
  virtual void deleteSelectedClips();

  //-----------------------------------------------------------------------------------------------
  // plugin handling:

  /** Adds an audio-plugin to this track.  */
  //virtual void addAudioPlugIn(AudioPluginInstance* pluginToAdd);

  /** Removes an audio-plugin from this Track. */
  //virtual void removeAudioPlugin(AudioPluginInstance* pluginToRemove, bool deleteObject);

  // \todo virtual void movePlugin - or maybe have an optional 2nd parameter "position" in 
  // addAudioPlugin which inserts the plugin at a specified position in the plugin chain - moving
  // would then be accomplished by remove/add

  /** Inserts the passed plugin instance into the plugin slot with given index. */
  //virtual void insertPlugin(int slotIndex, AudioPluginInstance* pluginToInsert);

  //-----------------------------------------------------------------------------------------------
  // audio related callbacks:

  /** Should be called before starting to call addSignalToAudioBlock - it will cause the 
  audioClipComponents to calculate their start and end times in samples. */
  virtual void prepareToPlay(double sampleRate);

  /** Generates the output of this track and adds it to the buffer, sliceStart represents the 
  global start time of the time slice to fill (measured in samples). */ 
  //virtual void addSignalToAudioBlock(const AudioSourceChannelInfo &bufferToFill, 
  //  const int sliceStartInSamples) const; // 1.46
  virtual void addSignalToAudioBlock(const AudioSourceChannelInfo &bufferToFill, 
    const int64 sliceStartInSamples) const;

  //-----------------------------------------------------------------------------------------------
  // state saving and recall:

  /** Returns the state (i.e. the settings of all relevant parameters) in form of an XmlElement. */
  virtual XmlElement* getStateAsXml(const File& songDirectory) const;

  /** Recalls a state (i.e. the settings of all relevant parameters) from an XmlElement. */
  virtual void setStateFromXml(const XmlElement& xmlState);

  //===============================================================================================
  juce_UseDebuggingNewOperator;

protected:

  /** Observers that needs to be informed, when something about this track has changed. */
  Array<TrackObserver*> observers;

  /** An array of (pointers to) the clips which are inside this track. */
  Array<AudioClip*, CriticalSection> audioClips;
    // todo: rename to "clips", let it be a baseclass pointer, get rid of the critical section
    // thread-sync should be managed centrally (maybe in class Arrangement or ArrangementEditor)

  /** Array of plugin slots which are to be applied to the output of the track */
  //Array<PluginSlot*, CriticalSection> pluginSlots;

  /** The chain of plugins to be applied to this track. */
  PluginChain pluginChain;

    // later, we may want to have midi-clips and midi-plugins (i.e. plugins that produce midi 
    // events as output) - in this case, the signal-flow should be: 
    // midiClips -> midiPlugins -> audioClips -> audioPlugins where the audioClips are optional
    // (i.e. there may be none) and the audioPlugins may also be instrument plugins


  String name;
  int    index;  
  bool   mute, solo; 
  double level, pan, amp;

};

#endif  