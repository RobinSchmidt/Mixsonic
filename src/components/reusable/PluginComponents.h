#ifndef PluginComponents_h
#define PluginComponents_h

#include "../../core/infrastructure/PluginChain.h"
#include "../widgets/RLabel.h"
#include "../widgets/RButton.h"


/** A Component subclass that wraps an AudioProcessorEditor into a window that can be placed on
the desktop, moved around, closed and re-opened. 

\todo provide widgets for bypass, dry/wet control, fxp/fxb load/save, etc.

*/

class AudioProcessorEditorContainer : public Component
{

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. You must pass a pointer to the AudioProcessorEditor to be wrapped. Depending on
  the "shouldTakeOwnership" parameter, this object with either delete the "editorToWrap" when 
  itself is deleted or not. If you pass "false", "editorToWrap" should be deleted elsewhere. */
  AudioProcessorEditorContainer(AudioProcessorEditor *editorToWrap, bool shouldTakeOwnership);

  /** Destructor */
  virtual ~AudioProcessorEditorContainer();


  //-----------------------------------------------------------------------------------------------
  // setup:

  virtual void showInFrontOfAppWindow();


  //-----------------------------------------------------------------------------------------------
  // inquiry:


  //-----------------------------------------------------------------------------------------------
  // callbacks:

  //virtual void focusLost(FocusChangeType cause);
  virtual void childBoundsChanged(Component *child);
  virtual void resized();
  virtual void paint(Graphics &g);

  //virtual void mouseDown(const MouseEvent &e);


protected:
   	
  SafePointer<AudioProcessorEditor> editor;
    // whenever the editor gets deleted, this SafePointer object will be automatically set to 
    // nullptr

  int titleBarHeight;
    // maybe factor this out into a class ComponentWithCustomTitleBar

  bool ownsEditor;
    // flag to indicate whether we should delete the wrapped editor or not when this object
    // is itself is deleted

  JUCE_LEAK_DETECTOR(AudioProcessorEditorContainer);
};

//=================================================================================================

/** A component to show a slot for an audio plugin which shows the name of the plugin and 
facilitates plugging in, replacing, plugging out, bypassing, opening the plugin GUI, etc.  */

class AudioPluginSlotComponent : public Component, public ChangeListener
{

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  AudioPluginSlotComponent(PluginSlot *pluginSlotToEdit, PluginChain *pluginChainToUse = nullptr);

  virtual ~AudioPluginSlotComponent();

  //-----------------------------------------------------------------------------------------------
  // inquiry:

  /** Returns true when our pluginSlot pointer is a nullpointer or (if it isn't) the 
  pluginSlot is empty. */
  virtual bool isEmpty();

  /** Returns true, when the (custom) editor is currently visible. */
  virtual bool isEditorVisible();

  /** Returns true, when the generic parameter editor is currently visible. */
  virtual bool isParameterEditorVisible();

  /** Returns true, when either the custom editor or the generic parameter editor or both are 
  currently visible. */
  virtual bool isAnyEditorVisible();

  //-----------------------------------------------------------------------------------------------
  // callbacks:
  virtual void changeListenerCallback(ChangeBroadcaster *source);
  virtual void mouseDown(const MouseEvent &e);
  virtual void resized();

  //-----------------------------------------------------------------------------------------------
  // misc:

  /** Updates the text in our Label to show the (possibly new) name of the plugin. */
  virtual void updateLabelText();

protected:

  /** Opens the popup menu which lets the user select plugins, bypass, etc. */
  virtual void openPopUpMenu();

  /** Opens the plugin's GUI editor, or a generic editor for GUI-less plugins. */
  virtual void openEditor();

  /** Opens a generic editor for the plugin's numeric parameters. */
  virtual void openParameterEditor();

  /** Wraps the passed AudioProcessorEditor into an AudioProcessorEditorContainer, the pointer to 
  which is passed by reference. This has the effect, that the "container" pointer will afterwards
  point to a newly created AudioProcessorEditorContainer object which contains the pluginEditor. */
  virtual void wrapPluginEditorIntoContainerAndShow(AudioProcessorEditorContainer* &container,                                                 
    AudioProcessorEditor* pluginEditor, bool shouldTakeOwnership);

  /** Makes the both editors (custom and generic) invisible but leaves the objects themselves 
  around. */
  virtual void hideEditors();

  //virtual void closeEditor();


  /** Opens a dialog where the user can pick the plugin to load. At the moment, this is a native 
  file-chooser dialog where the user picks the plugin's shared library file (i.e. the .dll, .dylib, 
  .so or whatever it is on the particular platform). */
  virtual void openLoadPluginDialog();

  /** Loads a plugin from the given file into our slot. */
  virtual void loadPluginFromFile(const File& pluginFile);

  /** Sets up the position of the passed editor to a meaningful position. The editor is passed as
  parameter, beacuse it may be either the generic parameter editor or the plugin's custom GUI. */
  virtual void setupEditorPosition(AudioProcessorEditorContainer *editor);



  /** Label to show the name of the plugin */
  RLabel *nameLabel;

  /** Pointer to the actual PluginSlot object which we edit. */
  PluginSlot *slotToEdit;

  /** Pointer to a PluginChain object in which our PluginSlot object sits. This chain object shall
  also take over ownership for the object pointed to by "slotToEdit". Only when chainToUse is a 
  nullptr, we will take care of the deletion of "slotToEdit" ourselves. */
  PluginChain *chainToUse;

  /** Pointer to a component that holds the actual plugin-editor. */
  AudioProcessorEditorContainer *editor;

  /** Pointer to a generic plugin parameter editor. */
  AudioProcessorEditorContainer *parameterEditor;


  JUCE_LEAK_DETECTOR(AudioPluginSlotComponent);
};

//=================================================================================================

/** A component showing an arbitrary number of AudioPluginSlotComponents in a vertical column. It
also takes care to always have an empty slot at the bottom, for a new plugin to be plugged in. */

class AudioPluginChainComponent : public Component, public ChangeListener
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

  //-----------------------------------------------------------------------------------------------
  // callbacks:

  // \todo on mouseOver show the full component (with all slots), even if some slots are hidden
  // due to a too small track-height - on mouseOver, it expands downward, if necesarry - but what
  // should we do, when there's not enough space below? expand upward? that might be weird because
  // the mouse won't be over the same slot as in the non-expanded state ...hmmm

  /** Updates all slot components, possibly deleting some and/or creating new ones. */
  virtual void updateSlotComponents();

  virtual void changeListenerCallback(ChangeBroadcaster* source);
  virtual void resized();

protected:

  PluginChain *pluginChain;

  int slotHeight; // height for one plugin slot

  JUCE_LEAK_DETECTOR(AudioPluginChainComponent);
};


#endif  