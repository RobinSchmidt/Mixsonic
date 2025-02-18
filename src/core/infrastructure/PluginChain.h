#ifndef PluginChain_h
#define PluginChain_h

#include "Lockable.h"
#include "DummyAudioPlugin.h"
#include "../../control/MixsonicGlobals.h"

/**

This is a class for representing a slot for plugins which may (or may not) contain an 
AudioPluginInstance and handles some additional settings such as bypass, etc.

*/

class PluginSlot : public Lockable, public ChangeBroadcaster
{  
  
  //friend class AudioPluginSlotComponent; // try to get rid of that

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  PluginSlot(AudioPluginInstance *pluginToUse, CriticalSection *mutexToUse);  

  /** Destructor. */
  ~PluginSlot();           

  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Loads a plugin specified by the plugin's shared library file into this slot and returns true, 
  if this was successful, false otherwise. */
  bool loadPlugin(const File& pluginFile);

  /** Loads a plugin specified by a PluginDescription into this slot and returns true, if this was 
  successful, false otherwise. */
  bool loadPlugin(const PluginDescription* description);

  /** Assigns a new plugin (instance) to this slot and optionally deletes the old one. */
  void setPlugin(AudioPluginInstance* pluginToUse, bool deleteOldPlugin);
    // \todo maybe get rid of the 2nd parameter - we should delete the old plugin in any case

  /** Switches bypass mode on/off */
  void setBypass(bool shouldBeBypassed);

  //-----------------------------------------------------------------------------------------------
  // inquiry:

  /** Returns a pointer to the underlying plugin instance. Note that this may be a nulltptr, 
  in case, this slot is empty. */
  AudioPluginInstance* getPlugin() const { return plugin; }

  /** Returns true, when there's currently no plugin in this slot. */
  bool isEmpty() const { return plugin == nullptr; }

  /** Returns true, if this slot is in bypass mode */
  bool isBypassed() const { return bypass; }

  //-----------------------------------------------------------------------------------------------
  // audio processing:

  /** Applies the plugin to the passed buffer. */
  virtual void processBlock(AudioSampleBuffer& buffer, MidiBuffer& midiMessages) const;

  /** Thread-unsafe version of processBlock. You may use this, if you already have acquired the
  mutex-lock for this object. */
  virtual void processBlockUnsafe(AudioSampleBuffer& buffer, MidiBuffer& midiMessages) const;

  //-----------------------------------------------------------------------------------------------
  // misc: 

  /** Returns the state as XmlElement. */
  XmlElement* getStateAsXml() const;

  /** Recalls a state from an XmlElement. */
  void setStateFromXml(const XmlElement& xmlState);

  /** Sets up the state of the plugin loaded from an encoded string (suitable for XML-storage). */
  void setPluginStateFromBase64Encoding(const String& stateString);

  /** Sets up the state of the plugin loaded from a MemoryBlock */
  void setPluginState(const MemoryBlock& state);

protected:

  /** Deletes the plugin instance that is used in this slot, thereby making sure that no editor
  is open. It also resets our pointer to null. */
  void deleteUnderlyingPlugin();

  /** Inserts a dummy plugin into the slot which serves as placeholder when a plugin is unavailable
  when we try to recall a state via setStateFromXml. */
  void insertDummyPlugin(const XmlElement& xmlState);

  /** Opens an alert box that asks the user to manually locate a plugin file. This will be opened, 
  when a plugin can't be found from its description or filename. The user can opt to not manually
  locate the file, in which case the function returns false, true otherwise. */
  static bool showPluginManualSearchAlert(const String& format, const String& name, 
    const String& manufacturer, const String& path);

  /** Opens an alert box that notifies the user that the attempted manually located plugin file is
  not the right one and gives the option to try again. If the user wants to loacte another file, 
  the function return true, false otherwise. */
  static bool showWrongPluginFileAlert(const String& format, const String& name, 
    const String& manufacturer, const String& path);

  /** Returns true, if the given plugin file has the given unique id, i.e. the file is (likely to 
  be) the right one for a plugin with given uid. */
  static bool doesPluginFileHaveUniqueId(const File& pluginFile, int uid);


  /** Pointer to the actual plugin instance object, maybe a nullptr in which case the slot is 
  empty. */
  AudioPluginInstance *plugin;

  bool bypass;  // maybe, we can replace this with a continuous dry/wet control (0.0 corresponds
                // to a bypassed setting)

  KnownPluginList *knownPlugins;

  JUCE_LEAK_DETECTOR(PluginSlot);
};

//=================================================================================================

/**

This is a class for representing chain (i.e. series connection) of plugins. 

*/

class PluginChain : public Lockable
{  

  friend class AudioPluginChainComponent;

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  PluginChain();  

  /** Destructor. */
  ~PluginChain();    

  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Deletes all PluginSlot objects in our array and clears the array itself. */
  void clear();

  /** Adds the passed slot to the end. */
  void addSlot(PluginSlot *slotToAdd);
    // \todo generalize this function into an "insertSlot" function which also takes an index at 
    // which  position it should be inserted (with -1 as default which means "at the end"). 

  /** Deletes the slot with given index.*/
  void deleteSlot(int index);

  /** Deletes the given slot object. */
  void deleteSlot(PluginSlot *slotToDelete);

  //-----------------------------------------------------------------------------------------------
  // inquiry:

  /** Returns true, when there's currently no pluginSlot in this chain or all pluginSlots in this 
  chain are empty. */
  bool isEmpty() const; 

  //-----------------------------------------------------------------------------------------------
  // audio processing:   
    
  /** Applies the plugin chain to the passed buffers. */
  void processBlock(const AudioSourceChannelInfo &bufferToFill, MidiBuffer &midiMessages) const;

  //-----------------------------------------------------------------------------------------------
  // misc: 

  /** Returns the state in form of an XmlElement. */
  XmlElement* getStateAsXml() const;

  /** Recalls a state from an XmlElement. */
  void setStateFromXml(const XmlElement& xmlState);

protected:

  Array<PluginSlot*> pluginSlots;
    // maybe we should use an OwnedArray

  CriticalSection mutex;
    // later, maybe have a pointer-to-CriticalSection and share it with the embedding Track

  JUCE_LEAK_DETECTOR(PluginChain);
};

// \todo getStateAsXml/setStateFromXml

#endif
