#ifndef Arrangement_h
#define Arrangement_h

#include "TimeFrame.h"
#include "Track.h"
#include "SamplePoolClient.h"
#include "SelectionListener.h"
#include <float.h>

/**

This class represents an arrangement, consisting of an arbitrary number of Tracks, each of which
can have an arbitrary number of Clips on them. For AudioClips, the underlying AudioFileBuffers are
centrally kept here in the Arrangement class in order to allow many clips to refer to the same 
underlying audio data.

\todo factor out an ArrangementPlayer class that holds a pointer to an Arrangement object as member 
and handles all the playback stuff (also the interfacing with the hardware via the audio callback)

\todo make an ArrangementObserver class that is called back whenever something has changed - use 
this to keep track of whether the arrangement is saved in the current state or not


*/

class Arrangement : public TrackObserver, public PositionableAudioSource, public TimeFrame, 
  public SamplePoolClient
{  

  friend class MixsonicArrangementPanel;
  friend class MixsonicArrangementEditor;
  friend class MixsonicContentComponent; 
    // this needs to access the tracks-array for setting up its MixsonicTrackControlComponents
    // but we soon want to move them into this component anyway

public:

  enum skipDirections
  {
    FORWARD = 0,
    BACKWARD
  };

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  Arrangement(SamplePool *samplePoolToUse);  

  /** Destructor. */
  virtual ~Arrangement();           

  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Initializes a fresh arrangement with 5 tracks and default settings for all parameters. */
  virtual void initializeArrangement();

  /** Selects one track to play in solo mode - pass -1 to turn solo off, indices out of range will
  be treated like -1. */
  //virtual void setTrackSolo(int trackToBeSoloed);

  //-----------------------------------------------------------------------------------------------
  // inquiry:

  /** Returns the master volume level in dB. */
  virtual double getMasterVolumeLevel() const;

  /** Overrides the method from TimeFrame in order to sync the value with the actual playback 
  pointer of the AudioSource base class. */
  virtual double getCurrentTimeInBeats() const;

  /** Overrides the method from TimeFrame in order to sync the value with the actual playback 
  pointer of the AudioSource base class. */
  virtual double getCurrentTimeInSeconds() const;

  /** Returns the end of the last clip in the whole arrangement or in the set of currently selected 
  clips in time unit of beats. */
  virtual double getLastClipEndTimeInBeats(bool onlySelectedClips);

  // \todo override the other related methods as well...

  //-----------------------------------------------------------------------------------------------
  // communication with other classes:

  /** Registers a SelectionListener (this listener will be stored and automatically registered with
  the restored clips when setStateFromXml is called). */
  virtual void registerSelectionListener(SelectionListener *newListener);

  //-----------------------------------------------------------------------------------------------
  // add and remove clips:

  /** De-selects all possibly the selected clips. */
  virtual void deSelectAllClips();

  /** Adds a clip to the track given by 'trackIndex'. If 'trackIndex' is inside the the range of 
  available tracks, the Arrangement will hand over the clip to the respective Track. If it is out 
  of range of available (that is: you try to add a Clip to a Track which doesn't exist), it will 
  return false. In that case, the caller must take care of the further lifetime of the Clip 
  (probably delete it). */
  virtual bool addClip(AudioClip* clipToAdd, int trackIndex);

  /** Cuts the selected Clips out of the arrangement and stores them in the clipboard. */
  virtual void cutSelectedClipsIntoClipboard(bool forgetOldClipboardContents = true);

  /** Copies (pointers to) all currently selected Clips into the clipboard (which is represented
  by our member clipsInClipboard). */
  virtual void copySelectedClipsIntoClipboard(bool forgetOldClipboardContents = true);

  /** Pastes the clips that are currently inside the clipboard into the Arrangement at the given 
  time instant in beats - if -1.0 is passed here, the current time position will be used (where the 
  time cursor currently is). The Clip with the smallest value for its position (the leftmost) will 
  be aligned to that time instant, other clips will be time-aligned so as to preserve the 
  time-difference to the leftmost. */
  virtual void pasteClipsFromClipboard(double timeInBeats = -1.0);

  /** Deletes the selected clips from the arrangement. */
  virtual void deleteSelectedClips();

  //-----------------------------------------------------------------------------------------------
  // navigate in the arrangement:

  /** Rewinds the arrangement to the beginning. */
  virtual void rewindToStart();

  /** Overrides the method from TimeFrame in order to sync the value with the actual playback 
  pointer of the AudioSource base class. */
  virtual void setCurrentTimeInBeats(double newCurrentTimeInBeats);

  /** Overrides the method from TimeFrame in order to sync the value with the actual playback 
  pointer of the AudioSource base class. */
  virtual void setCurrentTimeInSeconds(double newCurrentTimeInSeconds);

  //-----------------------------------------------------------------------------------------------
  // inquiry:

  /** Returns the number of tracks in the arrangement. */
  virtual int getNumTracks() const;

  /** Returns the current time position inside the arrangement as string for textual display. */
  virtual String getCurrentTimeAsString() const;

  //-----------------------------------------------------------------------------------------------
  // callbacks:

  /** Implements the purely virtual method from the SamplePoolClient baseclass in order to adjust 
  the filenames of all clips that use the the buffer which uses the given file. */
  virtual void samplePathHasChanged(int index, const File& oldFilePath, const File& newFilePath);

  /** Implements the purely virtual method from the SamplePoolClient baseclass in order to remove 
  all clips that use the the buffer which uses the given file. */
  virtual void sampleIsToBeDeleted(int index);

  /** When one track is put into solo-mode, we take care here do turn off the solo-mode for all
  other tracks. */
  virtual void trackSoloStateChanged(Track *track, bool isSolo);

  //-----------------------------------------------------------------------------------------------
  // audio related functions:

  /** Overrides the purely virtual method from PositionableAudioSource. Should be called before 
  starting to call addSignalToAudioBlock - it will cause the audioClipComponents to calculate their 
  start and end times in samples. */
  virtual void prepareToPlay(int samplesPerBlockExpected, double sampleRate);

  /** Switches audio-playback on/off. */
  virtual void play(bool shouldPlay);

  /** Sets up the master volume level. */
  virtual void setMasterVolumeLevel(double newMasterVolumeLevel);

  /** Overrides the purely virtual method from PositionableAudioSource. Does nothing */
  virtual void releaseResources();

  /** Overrides the purely virtual method from PositionableAudioSource. Fills a block with 
  data. */
  virtual void getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill);

  /** Overrides the purely virtual method from PositionableAudioSource. Sets the next position 
  for readout (in samples). */
  virtual void setNextReadPosition(int64 newPosition);

  /** Overrides the purely virtual method from PositionableAudioSource. Returns the next position 
  for readout (in samples). */
  virtual int64 getNextReadPosition() const;

  /** Overrides the purely virtual method from PositionableAudioSource. Returns the total length of 
  the arrangement (in samples). */
  virtual int64 getTotalLength() const;

  /** Overrides the purely virtual method from PositionableAudioSource. Returns false (for now) */
  virtual bool isLooping() const;

  //-----------------------------------------------------------------------------------------------
  // state saving and recall:

  /** Returns the state (i.e. the settings of all relevant parameters) in form of an XmlElement. 
  The argument must specify the directory into which the song project-file will be ultimately 
  written - this is needed to store the path of the underlying audio files of the clips as relative 
  path with respect to that song-directory. */
  virtual XmlElement* getStateAsXml(const File& songDirectory) const;

  /** Recalls a state (i.e. the settings of all relevant parameters) from an XmlElement. The 
  second argument must specify the directory from which the song project-file was loaded - this 
  is needed because the song has the path to the file on which this AudioClip relies stored as
  relative path with respect to the song's directory. */
  virtual void setStateFromXml(const XmlElement& xmlState, const File& songDirectory);

  //-----------------------------------------------------------------------------------------------
  // others:

  /** Aquires the mutex-lock for this Arrangement. */
  virtual void lockArrangement();

  /** Releases the mutex-lock for this Arrangement. */
  virtual void unlockArrangement();

  //===============================================================================================
  juce_UseDebuggingNewOperator;

protected:

  /** Initial number of tracks in the arrangement. */
  static const int numTracks = 8;

  /** Minimum length of the arrangement in beats. */
  static const int minLengthInBeats = 180;

  /** Threshold for auto-growing the length of the arrangement - when the end of the last 
  clip fals into range after the current arrangement length minus this value, the arrangement will 
  be expanded by autoGrowSize beats. The arrangement will also automatically shrink, when the 
  last clip's end is below the current length minus twice this value. */
  static const int autoGrowThreshold = 40;

  /** Size by which the arrangement automatically grows when the last clip's end exceeds current 
  length minus autoGrowThreshold. */
  static const int autoGrowSize = 40;

  /** Clears the clipboard (deletes the clips which are stored there and removes the pointers). */
  virtual void clearClipBoard();

  /** Updates our soloedTrack member variable according to which of the track is solo 
  (if any, -1, if none). */
  virtual void updateSoloedTrack();


  /** Overall length of the arrangement in beats. */
  //double lengthInBeats;

  /** This is the array the array of the tracks which make up the arrangement. */
  OwnedArray<Track, CriticalSection> tracks; 

  /** This is an array of pointers to clips which are currently inside the clipboard. */
  Array<AudioClip*> clipsInClipboard;

  /** This is the set of currently selected clips. */
  //SelectedItemSet<AudioClip*> selectedClips;

  SelectionListener* clipSelectionListener;

  /** A flag indicating whether or not we are currently playing back the arrangement. */
  bool isPlaying;

  /** The current position inside the arrangement in samples. */
  int64 positionInSamples;

  // The increment by which we skip forward or back ward when skipForward or skipBack are 
  // called:
  double defaultSkipInSeconds;

  // the master volume level (in dB) and the corresponding amplitude factor:
  double masterVolumeLevel;
  float  masterAmplitudeFactor;

  // the track which is currently in solo mode (-1, if none)
  int soloedTrack;

  // a mutex lock for accesses to the arrangement:
  CriticalSection lock;

};

#endif  