#ifndef PluginComponents_h
#define PluginComponents_h

#include "../../core/infrastructure/PluginChain.h"
#include "../../core/infrastructure/DeletionManagement.h"
#include "../../control/ActivationObserver.h"
#include "../widgets/RLabel.h"
#include "../widgets/RButton.h"


/** A Component subclass that wraps an AudioProcessorEditor into a window that can be placed on
the desktop, moved around, closed and re-opened. 

\todo provide widgets for bypass, dry/wet control, fxp/fxb load/save, etc.

*/

class AudioProcessorEditorContainer : public Component, public ActivationObserver, 
  public ButtonListener, public DeletionRequester
{

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. You must pass a pointer to the AudioProcessorEditor to be wrapped. Depending on
  the "shouldTakeOwnership" parameter, this object with either delete the "editorToWrap" when 
  itself is deleted or not. If you pass "false", "editorToWrap" should be deleted elsewhere. The 
  deletor should be some object that is responsible for deleting this object, when itself requests 
  this deletion (for example, by clicking on the close-button). The "ownerToWatchForActivation" is 
  supposed to be some window that has openend this editor window and should be watched for 
  (de)activation, to which this object responds with making itself (in)visible - it may be a 
  nullptr, in which case it won't become invisible.  */
  AudioProcessorEditorContainer(AudioProcessorEditor *editorToWrap, bool shouldTakeOwnership, 
     DeletionManager *deletor, Activatable *ownerToWatchForActivation = nullptr);

  /** Destructor */
  virtual ~AudioProcessorEditorContainer();


  //-----------------------------------------------------------------------------------------------
  // setup:

  virtual void showInFrontOfAppWindow();


  //-----------------------------------------------------------------------------------------------
  // inquiry:


  //-----------------------------------------------------------------------------------------------
  // callbacks:

  virtual void childBoundsChanged(Component *child);
  virtual void resized();
  virtual void paint(Graphics &g);
  virtual void activationStatusChanged(Activatable *activatable, bool isActive);
  virtual void buttonClicked(Button* button);
  virtual void mouseDown(const MouseEvent &e);
  virtual void mouseDrag(const MouseEvent &e);

protected:
   	
  SafePointer<AudioProcessorEditor> editor;
    // whenever the editor gets deleted, this SafePointer object will be automatically set to 
    // nullptr

  int titleBarHeight;
    // maybe factor this out into a class ComponentWithCustomTitleBar

  bool ownsEditor;
    // flag to indicate whether we should delete the wrapped editor or not when this object
    // is itself is deleted

  Activatable *visibilityController;
    // pointer to an object (usually a TopLevelWindow) that "owns" (in a sense) this plugin-editor 
    // window. we want to keep track of (de)activation of this owning window in order to ourselves
    // invisible on deactivation

  RLabel *nameLabel;
    // label to show the name of the plugin

  RButton *closeButton;
    // button to close the editor-window

  ComponentDragger dragger;

  JUCE_LEAK_DETECTOR(AudioProcessorEditorContainer);
};

//=================================================================================================

/** A component to show a slot for an audio plugin which shows the name of the plugin and 
facilitates plugging in, replacing, plugging out, bypassing, opening the plugin GUI, etc.  

\todo maybe factor out a class AudioProcessorEditorContainerOwner which manages all this open/close
business but is not a component, later, we may want to open/close plugin editors from places other
than such plugin-slot components, the AudioPluginSlotComponent may have only a pointer to such
an Owner object such that to each owner, more than one slot-component may be assigned (for example,
when an arrangement- and mixer-window should show the same plugin chain but open/use shared 
plugin-editors)

*/

class AudioPluginSlotComponent : public Component, public ChangeListener, public DeletionManager,
  public DeletionRequester
{

  friend class AudioPluginChainComponent;

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  AudioPluginSlotComponent(PluginSlot *pluginSlotToEdit);

  virtual ~AudioPluginSlotComponent();


  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Sets, whether or not this slot can be removed by the user (and provides a corresponding 
  option in the right-click popup menu). */
  virtual void setRemovable(bool shouldBeRemovable) { slotIsRemovable  = shouldBeRemovable; }


  virtual void setBypass(bool shouldBeBypassed);

  //-----------------------------------------------------------------------------------------------
  // inquiry:

  /** Returns true when our pluginSlot pointer is a nullpointer or (if it isn't) the 
  pluginSlot is empty. */
  virtual bool isEmpty();

  /** Returns true, when the (custom) editor is open. */
  virtual bool isCustomEditorOpen();

  /** Returns true, when the generic parameter editor is open. */
  virtual bool isParameterEditorOpen();

  /** Returns true, when either the custom editor or the generic parameter editor or both are 
  open. */
  virtual bool isAnyEditorOpen();

  //-----------------------------------------------------------------------------------------------
  // callbacks:
  virtual void changeListenerCallback(ChangeBroadcaster *source);
  virtual void mouseDown(const MouseEvent &e);
  virtual void resized();
  virtual void paintOverChildren(Graphics &g);
  virtual void handleDeletionRequest(DeletionRequester *objectThatWantsToBeDeleted);

  //-----------------------------------------------------------------------------------------------
  // misc:

  /** Updates the text in our Label to show the (possibly new) name of the plugin. */
  virtual void updateLabelText();

protected:

  /** Opens the popup menu which lets the user select plugins, bypass, etc. */
  virtual void openPopUpMenu();

  /** Opens a dialog where the user can pick the plugin to load. At the moment, this is a native 
  file-chooser dialog where the user picks the plugin's shared library file (i.e. the .dll, .dylib, 
  .so or whatever it is on the particular platform). */
  virtual void openLoadPluginDialog();

  /** Loads a plugin from the given file into our slot. */
  virtual void loadPluginFromFile(const File& pluginFile);

  /** Removes the plugin from our slot. */
  virtual void removePlugin();

  /** Opens the plugin's GUI editor, or a generic editor for GUI-less plugins. */
  virtual void openEditor();
  
  /** Opens the plugin's GUI editor. */
  virtual void openCustomEditor();

  /** Opens a generic editor for the plugin's numeric parameters. */
  virtual void openParameterEditor();

  /** Wraps the passed AudioProcessorEditor into an AudioProcessorEditorContainer, the pointer to 
  which is passed by reference. This has the effect, that the "container" pointer will afterwards
  point to a newly created AudioProcessorEditorContainer object which contains the pluginEditor. */
  virtual void wrapPluginEditorIntoContainerAndShow(AudioProcessorEditorContainer* &container,                                                 
    AudioProcessorEditor* pluginEditor, bool shouldTakeOwnership);

  /** Closes any open editor (custom or generic or both) */
  virtual void closeEditors();

  /** Closes the plugin's GUI editor (if open). */
  virtual void closeCustomEditor();

  /** Closes the plugin's generic parameter editor (if open). */
  virtual void closeParameterEditor();


  /** Label to show the name of the plugin */
  RLabel *nameLabel;

  /** Pointer to the actual PluginSlot object which we edit. */
  PluginSlot *slotToEdit;

  /** Pointer to a component that holds the actual plugin-editor. */
  AudioProcessorEditorContainer *customEditor;

  /** Pointer to a generic plugin parameter editor. */
  AudioProcessorEditorContainer *parameterEditor;

  /** Flag to indicate, if this slot is removable. */
  bool slotIsRemovable;


  JUCE_LEAK_DETECTOR(AudioPluginSlotComponent);
};

//=================================================================================================

/** A component showing an arbitrary number of AudioPluginSlotComponents in a vertical column. It
also takes care to always have an empty slot at the bottom, for a new plugin to be plugged in. */

class AudioPluginChainComponent : public Component, public ChangeListener, public DeletionManager
{

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  AudioPluginChainComponent(PluginChain *chainToEdit);

  virtual ~AudioPluginChainComponent();

  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Adds a slot-component to this chain. This object takes over responsibility for deleting it
  eventually. */
  //virtual void addSlotComponent(AudioPluginSlotComponent *newSlotComponent);
    // get rid of that - makes no sense anymore

  /** Deletes the components for all slots. */
  //virtual void deleteAllSlotComponents();

  // \todo provide functionality to change the order of the plugins by drag-and-drop





  virtual void removeLastSlot();



  //-----------------------------------------------------------------------------------------------
  // inquiry:


  virtual bool isLastSlotEmpty();

  virtual bool isSlotEmpty(int index);

  //virtual int getNumSlots


  //-----------------------------------------------------------------------------------------------
  // callbacks:

  // \todo on mouseOver show the full component (with all slots), even if some slots are hidden
  // due to a too small track-height - on mouseOver, it expands downward, if necesarry - but what
  // should we do, when there's not enough space below? expand upward? that might be weird because
  // the mouse won't be over the same slot as in the non-expanded state ...hmmm

  /** Updates all slot components, possibly deleting some and/or creating new ones. */
  virtual void updateSlotComponents();
  virtual void changeListenerCallback(ChangeBroadcaster* source);
  virtual void handleDeletionRequest(DeletionRequester *objectThatWantsToBeDeleted);
  virtual void resized();


protected:


  virtual void updateSize();

  PluginChain *pluginChain;

  PluginSlot *tempSlot;
    // a temporary PluginSlot object to be used to plugin new plugins at the end of the chain
    // it should always be empty, whenever it gets filled, it will be appended to the chain and
    // the "tempSlot" variable should be assigned to a new (empty) PluginSlot object

  AudioPluginSlotComponent *tempSlotComponent;
    // pointer to the component for the tempSlot


  int slotHeight; // height for one plugin slot

  JUCE_LEAK_DETECTOR(AudioPluginChainComponent);
};

#endif  
