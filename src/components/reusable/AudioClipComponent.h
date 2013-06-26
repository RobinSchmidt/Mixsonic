#ifndef AudioClipComponent_h
#define AudioClipComponent_h

#include "ClipComponent.h"
#include "DualWaveformDisplay.h"
#include "../../core/infrastructure/AudioClip.h"

/**

This class is a component which serves as graphical user interface to the AudioClip class allowing
to move the clip around, select/deselect it, set up the clip's level and fade-in/out settings via 
some handles and set up the active region via left/right border resizers.

*/

class AudioClipComponent : public ClipComponent, public AudioFileBufferUser
{  

public:

  /** Augment the enumeration of the draggable mouse handles. */
  enum mouseHandles
  {
    FADE_IN_HANDLE = ClipComponent::CLIP_NUM_MOUSEHANDLES,
    FADE_OUT_HANDLE,
    LEVEL_LINE,

    AUDIOCLIP_NUM_MOUSEHANDLES
  };

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  AudioClipComponent(AudioClip* newClipToEdit); 

  /** Copy Constructor. */
  AudioClipComponent(const AudioClipComponent &other);

  /** Destructor. */
  //virtual ~AudioClipComponent();                             

  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Assigns a new underlying AudioFileBuffer to this AudioClipComponent  
  \todo check, if this function is actually used - if not, remove (it doesn't seem to make sense)
  */
  virtual void assignAudioFileBuffer(AudioFileBuffer* newBuffer);

  /** Sets up the time range which is shown in this component (in seconds with respect to the 
  clips own time-axis). */
  virtual void setTimeRangeInComponent(double newMinTimeInSeconds, double newMaxTimeInSeconds);

  /** Constrains the time range which is to be plotted (in seconds with respect to the 
  clips own time-axis). */
  virtual void setTimeRangeToRender(double newMinTimeInSeconds, double newMaxTimeInSeconds);

  /** Overrides deleteUnderlyingClip() - do we still need to override this? */
  virtual void deleteUnderlyingClip();

  /** Sets the TimeSliceThread that will we used for redrawing. */
  virtual void setDrawingThread(TimeSliceThread* newDrawingThread);

  //-----------------------------------------------------------------------------------------------
  // inquiry:

  /** Returns a pointer to the underlying AudioClip. WARNING: Be aware that this may be a NULL 
  pointer and that the underlying clip may change or even be deleted in the lifetime of this
  AudioClipComponent object - try to avoid this function and if unavoidable, wrap accesses to
  the returned pointer in lockUnderlyingClip(), unlockUnderlyingClip(). */
  virtual AudioClip* getUnderlyingAudioClip();
    // check all calls to this function for being properly sourrounded by locks

  virtual File getUnderlyingFile();
    // see above - same here

  //-----------------------------------------------------------------------------------------------
  // callbacks:

  /** Overrides mouseMove in order to update the mouse cursor for different actions such as 
  dragging, adjusting the playback region, etc. */
  virtual void mouseMove(const MouseEvent &e);

  /** Overrides mouseDown in order to change the selection state of the clip on mouseclicks or 
  to grab one of the clip's handles or to grab the whole clip. */
  virtual void mouseDown(const MouseEvent &e);

  /** Overrides mouseDrag in order to drag the handles for clip level and fade-in/-out. */
  virtual void mouseDrag(const MouseEvent &e);

  /** Overrides clipIsToBeDeleted in order to NULL our audioClipToEdit pointer (which points to the
  same clip as the inherited clipToEdit pointer but is of type AudioClip*) and to pass NULL to the 
  embedded DualWaveformDisplay (indicating that nothing is there to be displayed) */
  virtual void clipIsToBeDeleted(Clip* clipToBeDeleted);

  /** Overrides paint. */
  virtual void paint(Graphics &g);

  /** Overrides the paint-function in order to overlay the waveform-display with the frame that
  indicates that this clips is selected and draws the fade-in/-out envelopes. */
  virtual void paintOverChildren(Graphics &g);

  /** Overrides the setBounds()-method of the Component base-class in order to arrange the 
  wave-displays according to the size - for best viewing it is advisable to use even heights. */
  virtual void resized();

  /** Overrides setMouseCursor in order to set the cursor also for our child-components. */
  virtual void setMouseCursor(const MouseCursor &cursorType);

  //-----------------------------------------------------------------------------------------------
  // thread synchronization:

  /*
  \todo check what this stuff was about and if it can be removed
  virtual void acquireWriteLocks();

  virtual void releaseWriteLocks();
  */

  /** This method replaces the lockUsedBufferPointer() from the baseclass AudioFileBufferUser in 
  order to acquire also the lock for AudioClip itself and the coreesponding locks in the 
  child-users. These locks belong all together here in this class. */
  virtual void lockClipAndBufferPointers();

  /** This method replaces the unlockUsedBufferPointer() from the baseclass AudioFileBufferUser in 
  order to acquire also the lock for AudioClip itself and the coreesponding locks in the 
  child-users. These locks belong all together here in this class. */
  virtual void unlockClipAndBufferPointers();

  //-----------------------------------------------------------------------------------------------
  // some delegations to the underlying AudioClip:

  virtual void setFadeInInSeconds(double newFadeInInSeconds);
  virtual void setFadeOutInSeconds(double newFadeOutInSeconds);

  // the embedded waveform display to show the waveform of the underlying audiofile:
  DualWaveformDisplay *waveDisplay; // \todo: move this back into the protected area again


  //===============================================================================================
  juce_UseDebuggingNewOperator;

protected:

  /** Does some initializations - used internally by the constructor and copy constructor. */
  virtual void initialize();

  /** Updates the waveform displays according to the underlying clip - you may call this when 
  something about the clip has been changed. */
  virtual void updateDisplay();

  /** Updates the content of the description field that is associated with this DescribedItem
  (if any). */
  virtual void updateDescriptionField();

  /** Converts a level in dB into an y-coordinate with respect to this Component. */
  virtual float levelToY(float level);

  /** Converts an y-coordinate with respect to this Component inot a level in dB. */
  virtual float yToLevel(float y);

  /** Returns the y-coordinate of the fade-in and -out handles with respect to this component. */
  virtual float getFadeHandleY();

  /** Assigns the reference parameters x,y to the x- and y coordinates of the center of handle for 
  the fade-in ramp (as coordinates with respect to this Component). */
  virtual void getFadeInHandleCenter(float &x, float &y);

  /** Assigns the reference parameters x,y to the x- and y coordinates of the center of handle for 
  the fade-out ramp (as coordinates with respect to this Component). */
  virtual void getFadeOutHandleCenter(float &x, float &y);

  /** Returns true when the point (x,y) is inside the handle for fade-in, false otherwise). */
  virtual bool isInsideFadeInHandle(float x, float y);

  /** Returns true when the point (x,y) is inside the handle for fade-out, false otherwise). */
  virtual bool isInsideFadeOutHandle(float x, float y);

  /** Returns true when the y coordinate is on the level line and the x coordinate is in between 
  the two fade handles, false otherwise. */
  virtual bool isOnLevelLine(float x, float y);

  /** Returns true when the point (x,y) is on the mute button, unless the muteButton is invisible 
  or the y coordinate of the point is on the levelLine. */
  virtual bool isOnMuteButton(float x, float y);

  /** Returns true when the point (x,y) is over the left border of the clip comoponent unless the 
  point is over a fade-handle. */
  virtual bool isOverLeftBorder(float x, float y);

  /** Returns true when the point (x,y) is over the right border of the clip comoponent unless the 
  point is over a fade-handle. */
  virtual bool isOverRightBorder(float x, float y);

  /** Returns the y-coordinate for the fade-in envelope at the left edge of this Component in
  components coordinates. */
  virtual float getEnvelopeAtLeftY();

  /** Returns the y-coordinate for the fade-out envelope at the right edge of this Component in
  components coordinates. */
  virtual float getEnvelopeAtRightY();

  // typecasted version of the inherited clipToEdit pointer
  AudioClip *audioClipToEdit;

  // the TimeSliceThread that will be used for redrawing the embedded WaveformDisplays:
  TimeSliceThread* drawingThread;

  // radius for the handles for the fade-in and -out functionality:
  static const int handleRadius = 5;

  // minimum and maximum levels for the clip's volume-level handle:
  static const double minLevel, maxLevel;

private:

  /** Overrides lockUsedBufferPointer() in private area to make it inaccessible - use 
  lockClipAndBufferPointers instead. */
  virtual void lockUsedBufferPointer() { AudioFileBufferUser::lockUsedBufferPointer(); }

  /** Overrides unlockUsedBufferPointer() in private area to make it inaccessible - use 
  unlockClipAndBufferPointers instead. */
  virtual void unlockUsedBufferPointer() { AudioFileBufferUser::unlockUsedBufferPointer(); }

  /** Overrides lockUnderlyingClip() in private area to make it inaccessible - use 
  lockClipAndBufferPointers instead. */
  virtual void lockUnderlyingClip() { ClipComponent::lockUnderlyingClip(); }

  /** Overrides unlockUnderlyingClip() in private area to make it inaccessible - use 
  lockClipAndBufferPointers instead. */
  virtual void unlockUnderlyingClip() { ClipComponent::unlockUnderlyingClip(); }

};

#endif  