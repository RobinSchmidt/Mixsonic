#ifndef TrackBodyComponent_h
#define TrackBodyComponent_h

#include "GlobalTimeFrameComponent.h"
#include "AudioClipComponent.h"
#include "../../core/infrastructure/Track.h"
#include "../../core/infrastructure/ClipInstantiationManager.h"
#include "../widgets/RButton.h"
#include "../widgets/MixsonicSlider.h"
#include "../widgets/RLabel.h"

/**

This class is a component which serves as graphical user interface to the AudioTrack class within 
the Mixsonic application.

*/

class TrackBodyComponent : public GlobalTimeFrameComponent, public ChangeListener, 
  public ClipInstantiationManager
{  

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  TrackBodyComponent(Track* newTrackToEdit);  

  /** Destructor. */
  virtual ~TrackBodyComponent();   

  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Overrides the method from TimeUnitConverter in order to re-align the clips. */
  virtual void setSelectedTimeUnit(int newSelectedTimeUnit);

  /** Sets the Label in which the descriptions for the clips will appear. */
  virtual void setDescriptionField(Label* newDescriptionField);

  /** Assigns a ComponentGrabber to all the ClipComponents on this TrackBodyComponent. */
  virtual void setComponentGrabber(ComponentGrabber* newGrabber);

  /** Sets up the background colour for this TrackBodyComponent. */
  virtual void setBackgroundColour(Colour newColour);

  /** Chooses one of the modes of operation for the mouse. @see ClipComponent::mouseModes */
  //virtual void setMouseMode(int newMouseMode);

  /** Adds an AudioClipComponent to this TrackBodyComponent, performing all required setup tasks such 
  as setting up the mouse-mode, description-field, ComponentGrabber, etc. */
  virtual void addAudioClipComponent(AudioClipComponent* componentToAdd);

  /** Removes an AudioClipComponent from this TrackBodyComponent, performing all required 
  setup/clear-up tasks. */
  virtual void removeAudioClipComponent(AudioClipComponent* componentToRemove);

  /** Clears the garbage arrays garbageClips and garbageComponents - this is automatically done
  in the destructor but may also be manually invoked through this function. */
  virtual void clearGarbage();

  /** Sets the TimeSliceThread that will we used for redrawing. */
  virtual void setDrawingThread(TimeSliceThread* newDrawingThread);

  //-----------------------------------------------------------------------------------------------
  // inquiry:

  /** Returns true, when there is some clip at the given x-coordinate (measured in this 
  TrackBodyComponent's coordinate system), false otherwise. */
  virtual bool isSomeClipAtX(int x) const;

  //-----------------------------------------------------------------------------------------------
  // user interface related callbacks:

  /** Clears the temporary selection state for all clips. */
  virtual void clearTemporarySelectionStates();

  /** Selects or deselects all clips. */
  virtual void selectAllClips(bool shouldBeSelected = true);

  /** Selects (or de-selects) all clips that lie wholly or partly inside the region between the 
  given x-coordinates measured in this TrackBodyComponent's coordinate system. */
  virtual void selectClipsInPixelRegion(bool shouldBeSelected, int x1, int x2, 
    bool temporarily = false);

  /** Clears the clipComponents array, removes (and deletes) these components from the list of 
  child-components and creates new ClipComponents for all the Clips on the underlying Track.*/
  virtual void refreshClipComponents();

  /** Checks whether we have for each clip a corresponding ClipComponent - if not, it creates the 
  missing ones. */
  virtual void updateClipComponents();

  /** A callback to indicate that some component was dropped onto this TrackBodyComponent, like - 
  for example - an AudioClipComponent. The passed x-value should be given with respect to the
  TrackCompoent's coordinate system will be used to position the clip. The return value informs, 
  whether or not this TrackBodyComponent has actually taken over the droppedComponent as its own 
  child-component. If true, the caller should NULL all its references to it but MUST NOT delete it
  - the TrackBodyComponent will from now on take care of the lifetime of the droppedComponent. If it 
  returns false, it means that the TrackBodyComponent was not interested in the droppedComponent - in 
  that case, the caller must take care of it's further lifetime by itself. */
  virtual bool dropComponent(Component* droppedComponent, int x);

  /** Assembles all the (pointers to) selected ClipComponents on this TrackBodyComponent into an Array 
  and returns that Array. When the second argument is true, it will create copies of the 
  ClipComponents and assemble those, otherwise, the Array will contain pointers to the original 
  ClipComponents. In either case, the caller is from now on responsible for the further lifetime 
  of the objects pointed to by the pointers in the Array. */
  virtual  Array<AudioClipComponent*> grabSelectedClips(bool grabCopy);

  /** Sets the time-range to be shown in seconds and arranges the clips accordingly. */
  virtual void setTimeRangeInSeconds(double newMinTime, double newMaxTime);

  /** Implements the purely virtual method of the ChangeListener baseclass in order to re-align the
  clip component that has called back this function. Clip components will send change messages 
  whenever theri active region changes. */
  virtual void changeListenerCallback(ChangeBroadcaster *objectThatHasChanged);

  /** Implements the purely virtual method of the ClipInstantiationManager baseclass. */
  virtual void handleDeletionRequest(Clip* clip);

  /** Implements the purely virtual method of the ClipInstantiationManager baseclass. */
  virtual void handleSplitRequest(Clip* clip, double timeInstantAtWhichToSplit, int timeUnit, 
    bool inLocalTime);

  /** Overrides the resized()-method of the Component base-class in order to arrange the clips 
  according to the size. */
  virtual void resized();

  /** Paints the track component. */
  virtual void paint(Graphics &g);

  juce_UseDebuggingNewOperator;

protected:

  /** Tries to find a ClipComponent in our array that is associated with the given Clip and returns 
  its index or -1 if none is found. To make it thread-safe, you should wrap a call to this function 
  together with all your subsequent use of the return value into 
  audioClipComponents.getLock().enter()/audioClipComponents.getLock().exit(). */
  virtual int findComponentForClip(Clip* clipToFindComponentFor);

  /** Deletes all the AudioClipComponents including some required additional operations, like 
  de-registering as orphanizationWatcher, etc. */
  virtual void clearAllClipComponents();

  /** Adjusts all the clips to the currently visibly time slice
  @see alignClipToTimeLine(AudioClipComponent* clipToAlign). */
  virtual void alignAllClipsToTimeLine(bool restrictBoundsToVisibleRange);

  /** Adjusts the clip with the given index to the currently visibly time slice
  @see alignClipToTimeLine(AudioClipComponent* clipToAlign). */
  virtual void alignClipToTimeLine(int clipIndex, 
    bool restrictBoundsToVisibleRange);

  /** Adjusts the passed clip to the currently visibly time slice (changing its position, width and 
  visibility, if necessary). If the second */
  virtual void alignClipToTimeLine(AudioClipComponent* clipToAlign, 
    bool restrictBoundsToVisibleRange);

  /** An array of (pointers to) the clips which are inside this track. */
  Array<AudioClipComponent*, CriticalSection> audioClipComponents;

  /** An owned array of pointers to Clip that are not used anymore because they requested their own 
  deletion (we can't directly delete them on such a request because then the object would indirecty 
  delete itself from within its own member function which is a severe bug). */
  OwnedArray<Clip, CriticalSection> garbageClips;

  /** An owned array of pointers to ClipComponents that are not used anymore. @see garbageClips */
  OwnedArray<AudioClipComponent, CriticalSection> garbageComponents;

  /** The underlying Track which is being edited. */
  Track *trackToEdit;

  /** The ComponentGrabber to grab the clips. */
  ComponentGrabber* grabber;

  /** We must store a pointer to the description field in order to assign it to created
  ClipComponents. */
  Label* descriptionField; 

  // the TimeSliceThread that will be used for redrawing the ClipComponents:
  TimeSliceThread* drawingThread;

  /** The background colour for the track. */
  Colour backgroundColour;

  // an index for the mode of operation of the mouse:
  int mouseMode;

};

//=================================================================================================

class AudioPluginSlotComponent : public Component
{

public:

  AudioPluginSlotComponent::AudioPluginSlotComponent();

  virtual AudioPluginSlotComponent::~AudioPluginSlotComponent();

  // void setPlugIn(AudioPluginInstance *newPlugIn);

  // return a component with a custom GUI - the caller should take over responsibility for further
  // lifetime
  //virtual Component* getCustomGUI();

  virtual void resized();

protected:

  RLabel *nameLabel;
  RButton *onOffButton;
  //RSlider *dryWetSlider; // maybe add later

  AudioPluginInstance *plugIn;
};

//=================================================================================================

class AudioPluginChainComponent : public Component
{

public:


  virtual AudioPluginChainComponent::~AudioPluginChainComponent();

  /** Adds a slot-component to this chain. This object takes over responsibility for deleting it
  eventually. */
  virtual void addSlotComponent(AudioPluginSlotComponent *newSlotComponent);

  // \todo provide functionality to change the order of the plugins by drag-and-drop

  // \todo on mouseOver show the full component (with all slot), even if some slots are hidden
  // due to a too small track-height - on mouseOver, it expands downward. if necesarry

  virtual void resized();

protected:

  //Array<AudioPluginSlotComponent*> slotComponents;


};

//=================================================================================================
/**

This class is a component that assembles some controllers for a Track such as sliders for level and 
panorama, mute- and solo buttons, etc.

\todo rename to TrackOutputComponent

*/

class MixsonicTrackControlComponent : public Component, public LabelListener, 
  public ButtonListener, public MixsonicSliderListener, public ChangeBroadcaster, 
  public TrackObserver 
{  

  friend class MixsonicArrangementEditor; 
    // needs to access the solo-button to implement mutual exclusivity among the tracks

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  MixsonicTrackControlComponent(Track* newTrackToEdit);  

  /** Destructor. */
  virtual ~MixsonicTrackControlComponent();                             

  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Sets the Label in which the descriptions for the widgets will appear. */
  virtual void setDescriptionField(Label* newDescriptionField);

  /** Sets up the widgets according to the parameters of the edited Track. */
  virtual void updateWidgetsAccordingToState();

  //-----------------------------------------------------------------------------------------------
  // inquiry:

  /** Returns the width which is desired to make this component look good. */
  static int getDesiredWidth() { return 160; }

  //-----------------------------------------------------------------------------------------------
  // callbacks:

  virtual void labelTextChanged(Label *labelThatHasChanged);
  virtual void buttonClicked(Button *buttonThatWasClicked);
  virtual void mSliderValueChanged(MixsonicSlider *mSliderThatHasChanged);
  virtual void resized();
  virtual void paint(Graphics &g);
  virtual void trackSoloStateChanged(Track *track, bool isSolo);


  juce_UseDebuggingNewOperator;

protected:

  /** Pointer to the track component, for which we implement some controls. */
  Track* trackToEdit;

  RLabel *nameLabel;
  RButton *muteButton, *soloButton;
  MixsonicSlider *levelSlider, *panSlider;
  AudioPluginChainComponent *pluginChainComponent;

private:

  // prevent copy constructor and operator= being generated:
  MixsonicTrackControlComponent (const MixsonicTrackControlComponent&);  
  const MixsonicTrackControlComponent& operator= (const MixsonicTrackControlComponent&);

};

//=================================================================================================

/** Assmbles a TrackControlComponent and TrackBodyComponent into a single object (as in the 
"Facade" pattern). */

class TrackComponent : public Component
{

  friend class MixsonicArrangementPanel; // maybe get rid of this and provide access functions

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  TrackComponent::TrackComponent(Track* newTrackToEdit);

  virtual TrackComponent::~TrackComponent();

  //-----------------------------------------------------------------------------------------------
  // manipulation:


  //-----------------------------------------------------------------------------------------------
  // inquiry:

  /** Returns the minimum desired height for track-components to look reasonable. */
  static int getMinimumHeight() { return 60; }

  /** returns the offset (in pixels) where the actual content area of the track begins. */
  virtual int getTrackBodyOffset() const { return MixsonicTrackControlComponent::getDesiredWidth(); }

  /** Returns true, iff the given x-coordinate is inside some area where a lasso-selection can
  be started (it should be on a track-body area where no clip is). */
  virtual bool canLassoSelectBeginHere(int x) const;

  //-----------------------------------------------------------------------------------------------
  // callbacks:

  virtual void resized();

  //-----------------------------------------------------------------------------------------------

  // delegations to the embedded TrackBodyComponent:
  virtual bool dropComponent(Component* droppedComponent, int x);
  virtual void selectClipsInPixelRegion(bool shouldBeSelected, int x1, int x2, 
    bool temporarily = false);

  juce_UseDebuggingNewOperator;

protected:

  TrackBodyComponent    *bodyComponent;
  MixsonicTrackControlComponent *outputComponent; // later call this outputComponent and also have an 
                                           // inputComponent

};





#endif  