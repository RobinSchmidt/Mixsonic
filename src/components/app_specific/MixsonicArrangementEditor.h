#ifndef MixsonicArrangementEditor_h
#define MixsonicArrangementEditor_h

#include "../../control/MixsonicGlobals.h"

#include "../reusable/ComponentGrabber.h"
#include "../reusable/UserInterfaceSection.h" // may be removed later
#include "MixsonicArrangementPanel.h"
#include "MixsonicArrangementScroller.h"
#include "MixsonicTrackControlComponent.h"
//#include "../widgets/MixsonicArrangementEditorResources.h"
#include "../widgets/MLabel.h"

/**

This class implements a GUI editor for an Arrangement object.

*/

class MixsonicArrangementEditor : virtual public Component, virtual public UserInterfaceSection, 
  public ButtonListener, public ChangeListener, public ComponentGrabber, public ChangeBroadcaster, 
  public Timer
{  

  friend class MixsonicContentComponent;

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  MixsonicArrangementEditor(SectionSkin *skinToUse, Arrangement* newArrangementToEdit, 
    SamplePool* samplePoolToUse);  

  /** Destructor. */
  virtual ~MixsonicArrangementEditor();           

  //-----------------------------------------------------------------------------------------------
  // setup: 

  /** Sets the Label in which the descriptions for the clips will appear. */
  virtual void setDescriptionField(MLabel* newDescriptionField);

  /** Sets the TimeSliceThread that will we used for redrawing. */
  virtual void setDrawingThread(TimeSliceThread* newDrawingThread);

  //-----------------------------------------------------------------------------------------------
  // inquiry:

  /** Returns the desired width of the clip in pixels according to the ratio of the clip's length 
  to the total length of the time-slice shown in the arrangementPanel. */
  virtual int getDesiredClipWidth(Clip *theClip) const;

  //-----------------------------------------------------------------------------------------------
  // callbacks:

  /** Overrides mouseUp in order to drop clips onto tracks and/or delete any temporary 
  AudioClipComponent. */
  virtual void mouseUp(const MouseEvent &e);

  /** Overrides mouseDrag in order to drage around the clips. */
  virtual void mouseDrag(const MouseEvent &e);

  /** Implements the purely virtual buttonClicked()-method of the ButtonListener base-class. */
  virtual void buttonClicked(Button *buttonThatWasClicked);

  /** Implements the purely virtual method of the ChangeListenerListener base-class.*/
  virtual void changeListenerCallback(ChangeBroadcaster *objectThatHasChanged);

  /** Implements the purely virtual method of the Timer base-class.*/
  virtual void timerCallback();

  /** Implements the purely virtual componentRequestsToBeGrabbed()-method of the ComponentGrabber
  base-class in order to grab clips from the arrangement. */
  virtual bool componentRequestsToBeGrabbed(GrabbableComponent* componentToBeGrabbed, 
    MouseEvent grabEvent);

  /** A callback to indicate that some component was dropped onto this MixsonicArrangementEditor, like, 
  for example, a ClipComponent (or, more precisely, some subclass thereof). The passed coordinates 
  should be with respect to this MixsonicArrangementEditor. The return value informs, whether or not 
  this MixsonicArrangementEditor (or a subcomponent thereof) has taken over the droppedComponent as its 
  own child-component. If true, the caller should NULL all its references to it but MUST NOT delete 
  it - the ArrangementPanel (or some subcomponent inside it) will from now on take care of
  the lifetime of the droppedComponent. If it returns false, it means that the ArrangementPanel 
  was not interested in the droppedComponent - in that case, the caller must take care of it's 
  further lifetime by itself. */
  virtual bool dropComponent(Component* droppedComponent, int x, int y);

  /** Overrides the setBounds()-method of the Component base-class in order to arrange the 
  widgets according to the size. */
  virtual void resized();

  //virtual void paint(Graphics &g);
  virtual void paintOverChildren(Graphics &g);

  /** Will be called from our arrangementPanel member when it wants to change its height due to
  zoom in/out. the rerun value indicates, if the new desired size could be granted. */
  virtual bool panelWantsSizeChange(Panel *panel, int desiredWidth, int desiredHeight);

  //-----------------------------------------------------------------------------------------------
  // state saving and recall: 

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

  //-----------------------------------------------------------------------------------------------
  // others:

  /** Initializes a new, fresh song with 5 tracks and default settings for all parameters. */
  virtual void initializeSong();

  /** Starts the playback of the arrangement at the current position. */
  virtual void startPlayback(int bufferSize, double sampleRate);

  /** Stops the playback of the arrangement. */
  virtual void stopPlayback();

  //==============================================================================================
  juce_UseDebuggingNewOperator;

protected:

  /** Returns true, when the point given by x,y in this component's coordinate system is inside
  the arrangement component, false otherwise. */
  bool isInsideArrangementPanel(int x, int y) const;

  /** Adjusts (automatically shrinks or grows if necesarry) the arrangement according to the end 
  time of the last clip in the arrangement. */
  virtual void adjustArrangementLength();

  /** Sets up the bounds of our embedded ArrangemntPanel with a desired width and height. The 
  return value informs, if the desired size could be granted. */
  virtual bool setupArrangementPanelBounds(int desiredWidth, int desiredHeight);

  // the main editing panel and it's navigation (zoom/scroll) component:
  MixsonicArrangementPanel *arrangementPanel;
  ArrangementScroller *arrangementNavigator;// rename to arrangementScroller

  MLabel *descriptionField;

  // these things are used to create and drag around a temporary clip object when an audio file
  // is clicked or some audio clip is grabbed from the arrangement:
  Component                  *draggedComponent;
  ComponentDragger           componentDragger;
  ComponentBoundsConstrainer unconstrainedBounds; // is this still used?
    // maybe, we should move this into MixsonicContentComponent - there, we have something like
    // this anyway and could treat it in a unified way from there

};

#endif  