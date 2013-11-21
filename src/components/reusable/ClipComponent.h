#ifndef ClipComponent_h
#define ClipComponent_h

#include "../../core/infrastructure/ClipManipulator.h"
#include "GrabbableComponent.h"
#include "Panel.h"
#include "../widgets/HalfTransparentButton.h"
#include "../../core/infrastructure/DescribedItem.h"

/**

This class is a component which serves as graphical user interface to the Clip class allowing
to move the clip around, select/deselect it and set up the active region via left/right border 
resizers.

*/

class ClipComponent : virtual public GrabbableComponent, virtual public Panel, public DescribedItem, 
  public ButtonListener, public ClipManipulator, public ChangeBroadcaster
{  

public:

  /** Enumeration of the draggable resizer handles inside this component. */
  enum mouseHandles
  {
    NONE = 0,
    LEFT_BORDER,
    RIGHT_BORDER,

    CLIP_NUM_MOUSEHANDLES
  };

  /** Enumeration of the different modes of operation for the mouse. */
  /*
  enum mouseModes
  {
    MOVE_CLIPS = 1,
    SPLIT_CLIP,
    DELETE_CLIPS
  };
  // deprecated
  */

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  ClipComponent(Clip* newClipToEdit); 

  /** Copy Constructor. */
  ClipComponent(const ClipComponent &other);

  /** Destructor. */
  virtual ~ClipComponent();                             

  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Sets up the time range which is shown in this component (in seconds with respect to the 
  clips own time-axis). */
  virtual void setTimeRangeInComponent(double newMinTimeInSeconds, double newMaxTimeInSeconds);

  /** Constrains the time range which is to be plotted (in seconds with respect to the clips own 
  time-axis) - as this class does not really render any graphics, this function is empty here, but 
  you may want to override it in your subclasses. */
  virtual void setTimeRangeToRender(double newMinTimeInSeconds, double newMaxTimeInSeconds);

  /** Sets the selection state and optionally sends a message to our listeners (by calling their
  selectionStateChange callback functions) and initiates repaint if necesarry. */
  virtual void setSelected(bool shouldBeSelected, bool temporarily,
    bool sendSelectionStateChangeMessage);

  /** Sets the temporary selection state and optionally sends a message to our listeners (by 
  calling their selectionStateChange callback functions) and initiates repaint if necesarry. */
  virtual void setTemporarilySelected(bool shouldBeTemporarilySelected,   
    bool sendSelectionStateChangeMessage = true);

  /** Chooses one of the modes of operation for the mouse. @see mouseModes */
  virtual void setMouseMode(int newMouseMode);

  //-----------------------------------------------------------------------------------------------
  // callbacks:

  /** Overrides clipNameChanged in order to update the caption of the ClipComponent. */
  virtual void clipNameChanged(Clip* clipThatHasChangedItsName);

  /** Overrides mouseMove in order to update the mouse cursor for different actions such as 
  dragging, adjusting the playback region, etc. */
  virtual void mouseMove(const MouseEvent &e);

  /** Overrides mouseDown in order to change the selection state of the clip on mouseclicks or 
  to grab one of the clip's handles or to grab the whole clip.  */
  virtual void mouseDown(const MouseEvent &e);

  /** Overrides mouseDrag in order to drag the handles for clip level and fade-in/-out. */
  virtual void mouseDrag(const MouseEvent &e);

  /** Overrides mouseUp in order to reset the currentlyDraggedHandle member to NONE. */
  virtual void mouseUp(const MouseEvent &e);
    
  /** Implements the purely virtual method of the ButtonListener base class. */
  virtual void buttonClicked(Button *buttonThatWasClicked);

  /** Overrides the paint-function in order to overlay the waveform-display with the frame that
  indicates that this clips is selected and draws the fade-in/-out envelopes. */
  virtual void paintOverChildren(Graphics &g);

  /** Overrides the setBounds()-method of the Component base-class in order to arrange the 
  wave-displays according to the size - for best viewing it is advisable to use even heights. */
  virtual void resized();

  /** Overrides paint in order to avoid dominance warning. */
  virtual void paint(Graphics &g);

  /** Overrides setMouseCursor in order to set the cursor also fo our child-components. */
  virtual void setMouseCursor(const MouseCursor &cursorType);

  //===============================================================================================
  juce_UseDebuggingNewOperator;

protected:

  /** Does some initializations - used internally by the constructor and copy constructor. */
  virtual void initialize();

  /** Updates the wavefrom displays according to the underlying clip - you may call this when 
  something about the clip has been changed. */
  virtual void updateDisplay();

  /** Returns true when the point (x,y) is on the mute button, unless the muteButton is invisible 
  or the y coordinate of the point is on the levelLine. */
  virtual bool isOnMuteButton(float x, float y);

  /** Returns true when the point (x,y) is over the left border of the clip comoponent unless the 
  point is over a fade-handle. */
  virtual bool isOverLeftBorder(float x, float y);

  /** Returns true when the point (x,y) is over the right border of the clip comoponent unless the 
  point is over a fade-handle. */
  virtual bool isOverRightBorder(float x, float y);

  // a text that will be drawn on top the clip component as caption:
  DrawableText caption;

  // indicates which of the handles is currently being dragged:
  int currentlyDraggedHandle;

  // a click field for muting the clip:
  HalfTransparentButton *muteButton;

  // an index for the mode of operation of the mouse:
  int mouseMode;

  // pointer to skin, defining colors, font, etc.:
  Skin *skin;

};

#endif  