#ifndef MixsonicArrangementPanel_h
#define MixsonicArrangementPanel_h

#include "../reusable/RectangleComponent.h"
#include "../reusable/TimeLineComponent.h"
#include "../reusable/TrackComponent.h"
#include "MixsonicTrackControlComponent.h"
#include "../reusable/ComponentDragContainer.h"
#include "../reusable/Panel.h"
#include "../../core/infrastructure/Arrangement.h"
#include "../../core/infrastructure/SelectionListener.h"
#include "../widgets/RComboBox.h"
#include <limits.h>

/**

This class is a component for arranging ClipComponents inside TrackComponents.

*/

class MixsonicArrangementPanel : public Panel, public SelectionListener, public SamplePoolClient, 
  public ChangeListener, public ChangeBroadcaster
{  

  friend class MixsonicContentComponent;
  friend class MixsonicArrangementEditor;

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  MixsonicArrangementPanel(Arrangement* newArrangementToEdit, SamplePool* samplePoolToUse);  

  /** Destructor. */
  virtual ~MixsonicArrangementPanel();                             

  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Initializes a fresh arrangement with 5 tracks and default settings for all parameters. */
  virtual void initializeArrangement();

  /** Sets up the song tempo. */
  virtual void setTempoInBpm(double newTempo);

  /** Sets up the time unit to be used by the timeline ruler. @see TimeUnitConverter::timeUnits */
  virtual void setTimeUnit(int newTimeUnit);

  /** Sets the Label in which the descriptions for the widgets will appear. */
  virtual void setDescriptionField(Label* newDescriptionField);

  /** Assigns a ComponentGrabber to all the ClipComponents on this MixsonicArrangementPanel. */
  virtual void setComponentGrabber(ComponentGrabber* newGrabber);

  /** Adjusts the position of the time cursor according to the value of currentTime in the timeLine
  component. */
  virtual void adjustTimeCursorPosition();

  /** Updates the GUI Editor according to the current playback position in the edited arrangement.
  This may result in an automatic scrolling of the Panel if necesarry and it also includes a call 
  to adjustTimeCursorPosition(). */
  virtual void updateGuiAccordingToPlaybackPosition();

  /** Chooses one of the modes of operation for the mouse. @see mouseModes */
  //virtual void setMouseMode(int newMouseMode);
    // deprecated

  /** Sets the TimeSliceThread that will we used for redrawing. */
  virtual void setDrawingThread(TimeSliceThread* newDrawingThread);

  /** Sets the height (in pixels) to be used for one track. */
  virtual void setTrackHeight(int newHeight);

  //-----------------------------------------------------------------------------------------------
  // inquiry:

  /** Returns the desired width of the clip in pixels according to the ratio of the clip's length 
  to the total length of the time-slice shown in the arrangementPanel. */
  virtual int getDesiredClipWidth(Clip *theClip) const;

  /** Returns true, iff the given coordinates are inside some area where a lasso-selection can
  be started (it should be on a track-body area where no clip is). */
  virtual bool canLassoSelectBeginHere(int x, int y) const;

  /** Returns true, when the current playback position inside the arrangement is behind the end of 
  the arrangement, as defined by the maximum visible time (when zoomed out or with the scrollbar 
  being scrolled to hard right) - this may be used to stop the playback. */
  virtual bool isCurrentTimeBehindEndOfArrangement();

  /** Returns the y-coordinate, where the area for the tracks begins (it's below the timeline). */
  virtual int getTrackAreaY() const;

  /** Returns the height of one track-component in pixels. */
  virtual int getTrackHeight() const { return trackHeight; }

  /** Returns the total height required for all tracks in pixels. */
  virtual int getHeightForAllTracks() const;

  //-----------------------------------------------------------------------------------------------
  // callbacks:

  /** Implements the purely virtual method form the SamplePoolClient baseclass. */
  virtual void samplePathHasChanged(int index, const File& oldFilePath, 
    const File& newFilePath);

  /** Implements the purely virtual method form the SamplePoolClient baseclass in order to update 
  the TrackComponents when a clip was deleted due to deletion of a sample from the pool. */
  virtual void sampleIsToBeDeleted(int index);

  /** Overrides mouseDown. */
  virtual void mouseDown(const MouseEvent &e);

  /** Overrides mouseDrag. */
  virtual void mouseDrag(const MouseEvent &e);

  /** Overrides mouseUp. */
  virtual void mouseUp(const MouseEvent &e);

  /** Implements the purely virtual method of the ChangeListenerListener base-class.*/
  virtual void changeListenerCallback(ChangeBroadcaster *objectThatHasChanged);

  /** Overrides the method of the SelectionListener base class. */
  virtual void selectionStateChangedByMouse(SelectableItem *itemThatHasChanged, 
    const MouseEvent& e);

  //-----------------------------------------------------------------------------------------------
  // user interface related functions:

  /** Clears the temporary selection state for all clips. */
  virtual void clearTemporarySelectionStates();

  /** Selects or deselects all clips. */
  virtual void selectAllClips(bool shouldBeSelected = true);

  /** Clears the trackComponents array, removes these components from the list of child-components.
  This should be called before a new song is being loaded in order to not have TrackComponents lying 
  around which may refer to Tracks that have already been deleted. */
  virtual void clearTrackComponents();

  /** Fills the trackComponents array according to the Arrangement which is being edited - this 
  function should be called, when the arrangement has been changed in such that the current 
  configuration of child-components does not reflect it anymore - as after loading a new song, for 
  example. */
  virtual void createTrackComponents();

  /** Causes the TrackComponents to check whether they are up to date (have a clip component for 
  each of the clips) - if not they will create the missing ClipComponents. */
  virtual void updateTrackComponents();

  /** A callback to indicate that some component was dropped onto this MixsonicArrangementPanel, like, 
  for example, a ClipComponent (or, more precisely, some subclass thereof) or a 
  ComponentDragContainer containing a number of those ClipComponents. The passed coordinates 
  should be with respect to this MixsonicArrangementPanel. The return value informs, whether or not 
  this MixsonicArrangementPanel (or a subcomponent thereof) has taken over responsibility for the 
  droppedComponent. If true, the caller should NULL all its references to it but MUST NOT delete 
  it - the MixsonicArrangementPanel (or some subcomponent inside it) will from now on take care of
  the lifetime of the droppedComponent. If it returns false, it means that the MixsonicArrangementPanel 
  was not interested in the droppedComponent - in that case, the caller must take care of it's 
  further lifetime by itself. Whether or not the MixsonicArrangementPanel takes over responsibility 
  depends on the actual type (subclass) of the passed component. */
  virtual bool dropComponent(Component* droppedComponent, int x, int y);

  /** This function can be used to drop a Component specifically of class AudioClipComponent. It is 
  mainly intended to be used internally by dropComponent(). */
  virtual bool dropAudioClipComponent(AudioClipComponent* droppedClipComponent, int x, int y);

  /** Assembles the currently selected ClipComponents into a ComponentDragContainer (which then 
  also becomes the parent Component for them) and returns a pointer to it. The caller is 
  responsible to delete this object when it's not needed anymore (which will also delete the 
  ClipComponents because they are child-components of the container). Of course, the caller can 
  also re-delegate the lifetime responsibility to the MixsonicArrangementPanel by dropping the container
  on it again. If no clips are selected, it will return an empty container. */
  //virtual ComponentDragContainer* grabSelectedClipsAt(int x, int y, bool grabCopy = false); // 1.46
  virtual ComponentDragContainer* grabSelectedClips(bool grabCopy = false);

  /** Cuts the selected Clips out of the arrangement and stores them in the clipboard. */
  virtual void cutSelectedClipsIntoClipboard(bool forgetOldClipboardContents = true);

  /** Copies (pointers to) all currently selected Clips into the clipboard. */
  virtual void copySelectedClipsIntoClipboard(bool forgetOldClipboardContents = true);

  /** Pastes the clips that are currently inside the clipboard into the Arrangement at the given 
  time instant in beats - if -1.0 is passed here, the current time position will be used (where the 
  time cursor currently is). The Clip with the smallest value for its position (the leftmost) will 
  be aligned to that time instant, other clips will be time-aligned so as to preserve the 
  time-difference to the leftmost. */
  virtual void pasteClipsFromClipboard(double timeInBeats = -1.0);

  /** Deletes the selected clips from the arrangement. */
  virtual void deleteSelectedClips();

  /** Returns a pointer to a track with the given index, or NULL if no track with such an index 
  exists. This does not detach the comonpent from this MixsonicArrangementPanel - the TrackComponent
  to which the pointer points will remain a child-component of this MixsonicArrangementPanel. */
  virtual TrackComponent* getTrackComponent(int trackIndex);

  /** Adjusts the current position along the timeline according to some x-coordinate given in 
  pixels with respect to this ArrangementComponent. */
  virtual void gotoTimeLinePixelX(int x);

  /** Overrides the Panel's method to set up the minimum and maximum time axis values that 
  should be visible. */
  virtual void setCurrentRangeX(double newMinX, double newMaxX);

  /** Overrides the Panel's method to set up the positions and heights of the TrackComponents. */
  virtual void setCurrentRangeY(double newMinY, double newMaxY);

  /** Overrides the setBounds()-method of the Component base-class in order to arrange the 
  widgets according to the size. */
  virtual void resized();

  /** Overrides paintOverChildren in order to possibly draw the lasso rectangle. */
  virtual void paintOverChildren(Graphics& g);

  //-----------------------------------------------------------------------------------------------
  // song recall:

  /** Returns the state (i.e. the settings of all relevant parameters) in form of an XmlElement. 
  The argument must specify the directory into which the song project-file will be ultimately 
  written - this is needed to store the path of the underlying audio files of the clips as relative 
  path with respect to that song-directory. */
  virtual XmlElement* getStateAsXml(const File& songDirectory) const;

  /** Recalls a state (i.e. the settings of all relevant parameters) of the edited Arrangement from 
  an XmlElement and takes care for updating the GUI components accordingly. The second argument 
  must specify the directory from which the song project-file was loaded - this is needed because 
  the song has the path to the file on which this AudioClip relies stored as relative path with 
  respect to the song's directory. */
  virtual void setStateFromXml(const XmlElement& xmlState, const File& songDirectory);

  //===============================================================================================
  juce_UseDebuggingNewOperator;

protected:

  /** Determines, which one inside the array of the trackComponents is closest to a given 
  y-coordinate - it returns -1 if the array is emptyor the y-coordinate is above the timeLine. */
  virtual int getClosestTrackComponent(int y) const;

  /** Sets up the bounds of the TrackComponenst and TrackControlCompoenents according to the 
  currently visible range along the y-coordinate. */ 
  virtual void setupTrackBounds();

  /** Returns the enclosing rectangle for a number of AudioClipComponents in coordinates with 
  respect to this MixsonicArrangementPanel. */
  virtual Rectangle<int> getEnclosingRectangle(Array<AudioClipComponent*> clipComponents);

  /** Converts the coordinates of the ClipComponents (which are assumed to be with respect to the 
  TrackComponent which hosts the respective ClipComponent) to new coordinates with respect to an 
  arbitrary rectangle (whose coordinates are assumed with respect to this MixsonicArrangementPanel). */
  virtual void convertClipBoundsToRectangle(Array<AudioClipComponent*> clipComponents, 
    Rectangle<int> rectangle);

  /** Selects all clips that are (wholly or partly) inside the given rectangle. The rectangle has 
  to be given in this component's coordinates. The second argument specifies, whether this 
  selection should be only temporarily. */
  void selectClipsInRectangle(Rectangle<int> selectionRectangle, bool temporarily);

  /** Function to update the maximum- and current range for the y-coordinate of the inherited Panel 
  object. The maximum range should always go from 0 to the number of height-pixels required to fit 
  all tracks exactly. The current range should reflect what is currently seen. You should call this 
  function whenever this maximum range might change (on inserting/removing tracks, changing the 
  height of the tracks, etc.) */
  void updatePanelRangeY();

  /** Our TrackComponents */
  Array<TrackComponent*, CriticalSection> trackComponents;

  RectangleComponent *timeCursor;     // just a vertical line
  TimeLineComponent  *timeLine;       // the timeline above the TrackComponents

  /** The ComponentGrabber to grab the clips. */
  ComponentGrabber *grabber;

  /** The description field for all the GUI components. */
  Label *descriptionField; 

  // stuff for lasso selection:
  RectangleComponent* lassoRectangle;
  Array<AudioClipComponent*> lassoSelectedClips; // is this necesarry?

  /** The arrangement that is being edited. */
  Arrangement* arrangementToEdit;

  // the TimeSliceThread that will be used for redrawing the ClipComponents:
  TimeSliceThread* drawingThread;

  int trackHeight;
  double trackOffsetY; // the offset along the Y-axis in units of "number-of-tracks"

  // maybe make these non-static:
  static const bool   autoScrollActive       = true;         
  static const double autoScrollThreshold;
  static const int    autoScrollStepInPixels = 100;  

};

#endif  