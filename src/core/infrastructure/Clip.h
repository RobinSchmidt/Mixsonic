#ifndef Clip_h
#define Clip_h

#include "TimeUnitConverter.h"
#include "SelectableItem.h"
class ClipDeletionWatcher;
class ClipInstantiationManager;

/**

This class represents a clip that can be used in audio sequencer applications. It assumes that 
there exists some underlying timeline in the sequencer which represents the global time axis. Along
this global timeline, the clip can be positioned and an active region can be defined, where the 
active region contains the data that will actually be played back. Data in inactive regions (pre 
and post the active region) is assumed to be dead (whatever that means for the particular subclass 
of clip at hand - for AudioClips, it will mean to mute the data, for example).

*/

class Clip : public TimeUnitConverter, public SelectableItem
{  

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  Clip();  

  /** Copy Constructor. Copies all data from the other clip except the array of 
  ClipDeletionWatchers. */
  Clip(const Clip& other);  

  /** Destructor. */
  virtual ~Clip();                             

  //-----------------------------------------------------------------------------------------------
  // callbacks:

  /** Should be called before playing back an Clip and overriden in subclasses to do 
  initializations that are necesarry prior to actual playback. Playback itself is then done via 
  callbacks to addSignalToAudioBlock(). */
  virtual void prepareToPlay(double sampleRate);

  /** This function should be overriden to add the signal that will be created by the clip to some
  buffer. sliceStart represents the global start time of the time slice to fill (measured in 
  samples). The ampL and ampR are used as amplitude factors for the left and right channels 
  signal. */
  virtual void addSignalToAudioBlock(const AudioSourceChannelInfo &bufferToFill, 
    const int sliceStartInSamples, const double ampL = 1.0, const double ampR = 1.0);

  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Initializes all members to default values. */
  virtual void init();

  /** Sets the name of the clip. */
  virtual void setName(const String& newName);

  /** Sets the position of the clip along the global timeline in the unit that will be passed by 
  the second parameter. For possible units see @see TimeUnitConverter::timeUnits. */
  //virtual void setPosition(double newPosition, int timeUnit); // juce 1.46
  virtual void setClipPosition(double newPosition, int timeUnit);

  /** Sets the start of the clip's active-region. The third argument specifies whether the passed 
  value should be interpreted as being with respect to the Clip's local time axis (true), or with 
  respect to the underlying global time axis (false). 
  @see setPosition, setActiveRegionEnd */
  virtual void setActiveRegionStart(double newStart, int timeUnit, bool inLocalTime);

  /** Sets the end of the clip's active-region. @see setPosition, setActiveRegionStart  */
  virtual void setActiveRegionEnd(double newEnd, int timeUnit, bool inLocalTime);

  /** Stores the index Track on which this Clip is hosted when we assume an arrangement with 
  multiple tracks - this function should be called whenever the clip is moved (or initially 
  dropped) onto a Track, or when the ordering of the Tracks changes, in order to allow the Clip to 
  know on which Track it is. Pass -1 here, when the clip is not hosted on any valid Track (for 
  example when it exists only in the clipboard). Letting the clip know on which track it is 
  facilitates copy-and-paste functionality. */
  virtual void setHostingTrackIndex(int newHostingTrackIndex);

  /** Sets the mute-flag for this Clip true or false. */
  virtual void setMute(bool shouldBeMuted) { mute = shouldBeMuted; }

  /** Adds an AudioClipDeletionWatcher to this AudioClip. The deletionWatcher will be called back 
  when this object is going to be deleted. */
  virtual void addClipDeletionWatcher(ClipDeletionWatcher* watcherToAdd);

  /** Removes an AudioClipDeletionWatcher from this AudioClip object. */
  virtual void removeClipDeletionWatcher(ClipDeletionWatcher* watcherToRemove);

  /** Removes all AudioClipDeletionWatchers from this AudioClip object. */
  virtual void removeAllClipDeletionWatchers();

  //-----------------------------------------------------------------------------------------------
  // inquiry:

  /** Returns the name of the clip. */
  virtual String getName() const;

  /** Returns the position time of the clip along the global time axis. @see setPosition */
  //virtual double getPosition(int timeUnit) const;  // juce 1.46
  virtual double getClipPosition(int timeUnit) const; 

  /** Returns the total duration of the clip (counting the active region as well as the heading and 
  trailing inactive regions). */
  virtual double getTotalDuration(int timeUnit) const;

  /** Returns the duration of the active region of the clip. */
  virtual double getActiveRegionDuration(int timeUnit) const;

  /** Returns the start of the clip's active region. @see setActiveRegionStartInSeconds(). */
  virtual double getActiveRegionStart(int timeUnit, bool inLocalTime) const;

  /** Returns the end of the clip's active region. @see setActiveRegionEndInSeconds(). */
  virtual double getActiveRegionEnd(int timeUnit, bool inLocalTime) const;

  /** Returns true when a time instant is inside the clip, false otherwise. */
  virtual bool isTimeInstantInsideClip(double timeInstant, int timeUnit, bool inLocalTime) const;

  /** Returns true when a time instant is inside the active region of the clip, false otherwise. */
  virtual bool isTimeInstantInsideActiveRegion(double timeInstant, int timeUnit, 
    bool inLocalTime) const;

  /** Informs whether this clip has data inside a time slice (measured in beats) between 
  timeInstant1 and timeInstant2. \todo: test this function */
  virtual bool hasDataBetweenTimeInstants(double timeInstant1, double timeInstant2, int timeUnit, 
    bool inLocalTime) const;

  /** Informs whether this clip has data inside a time slice (measured in beats) between 
  timeInstant1 and timeInstant2 but only if the timeslice is also inside the active region of the 
  clip. */
  virtual bool hasActiveDataBetweenTimeInstants(double timeInstant1, double timeInstant2, 
    int timeUnit, bool inLocalTime) const;

  /** Returns the Track on which this Clip is hosted - initially this will be -1 and should be set 
  up via setHostingTrackIndex(). */
  virtual int getHostingTrackIndex() const;

  /** Indicates whether or not this Clip is muted. */
  virtual bool isMuted() const { return mute; }

  //-----------------------------------------------------------------------------------------------
  // callback based instantiation management:

  /** Sets the ClipInstantiationManager object that will be called back when this clips sends out
  requests about being deleted, splitted, etc. @see sendDeletionRequest, sendSplitRequest. */
  virtual void setInstantiationManager(ClipInstantiationManager* newInstantiationManager);

  /** Sends a request to the instantiationManager (if any) that this Clip wants to be deleted. This 
  will typically not directly this clip (deletion of an object from within itself is not a good 
  idea) but move it into garbage area for later deletion. */
  virtual void sendDeletionRequest();

  /** Sends a request to the instantiationManager (if any) that this Clip wants to be splitted 
  into two clips at some time instant. */
  virtual void sendSplitRequest(double timeInstantAtWhichToSplit, int timeUnit, bool inLocalTime);

  //-----------------------------------------------------------------------------------------------
  // state saving and recall:

  /** Returns the state (i.e. the settings of all relevant parameters) in form of an XmlElement. */
  virtual XmlElement* getStateAsXml() const;

  /** Recalls a state (i.e. the settings of all relevant parameters) from an XmlElement. */
  virtual void setStateFromXml(const XmlElement& xmlState);

  //===============================================================================================
  juce_UseDebuggingNewOperator;

protected:

  /** Converts between local and global time axes and various time units. */
  double convertTimeUnit(const double in, const int inUnit, const bool inTimeIsLocal,
    const int outUnit, const bool outTimeIsLocal) const;

  /** The name of the clip. */
  String name;

  /** The position of the clips in unit of beats measured in global time. */
  double position;

  /** The total duration of the clip (counting the active region as well as the heading and 
  trailing inactive regions) in unit of beats. */
  double totalDuration; 

  /** The duration of the inactive region before the active region begins in unit of beats. */
  double headingInactiveDuration; 
  
  /** The duration of the inactive region after the active region end in unit of beats. */
  double trailingInactiveDuration;

  /** Flag to indicate that the clip is muted. */
  bool mute;

  /** The index of the track which hosts this clip - keeping this info inside the clip is handy for
  copy/paste operations. */
  int hostingTrackIndex;

  /** The objects that need to be notified when this clips is going to be deleted (i.e. objects 
  that hold references to this Clip). */
  Array<ClipDeletionWatcher*, CriticalSection> deletionWatchers;

  /** An object can be called back with requests for certain kinds of manipulations that have to do 
  with the instantiation (construction/destruction) of Clip objects because such things cannot be 
  managed from inside this class but instead have to be implemented by an outlying class that 
  embedds Clip objects. */
  ClipInstantiationManager* instantiationManager;

  // ATTENTION: if you add more member variables here, make sure that you copy their values in the 
  // copy-constructors of all subclasses in the hierarchy (class AudioClip, for example, defines 
  // its own copy-constructor).

};

#endif  