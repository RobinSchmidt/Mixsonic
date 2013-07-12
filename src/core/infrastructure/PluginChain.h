#ifndef PluginChain_h
#define PluginChain_h

#include "Lockable.h"

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

  /** Loads a plugin specified by the plugin's shared library file into this slot. If the library 
  can't be loaded as plugin, nothing will be done and an error message is shown. */
  void loadPlugin(const File& pluginFile);

  /** Loads a plugin specified by a PluginDescription into this slot. */
  void loadPlugin(const PluginDescription* description);

  /** Assigns a new plugin (instance) to this slot and optionally deletes the old one. */
  void setPlugin(AudioPluginInstance* pluginToUse, bool deleteOldPlugin);

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
  virtual void processBlock(AudioSampleBuffer& buffer, MidiBuffer& midiMessages);

protected:

  /** Deletes the plugin instance that is used in this slot, thereby making sure that no editor
  is open. It also resets our pointer to null. */
  void deleteUnderlyingPlugin();

  /** Pointer to the actual plugin instance object, maybe a nullptr in which case the slot is 
  empty. */
  AudioPluginInstance *plugin;

  bool bypass;  // maybe, we can replace this with a continuous dry/wet control (0.0 corresponds
                // to a bypassed setting)

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
  virtual ~PluginChain();    

  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Deletes all PluginSlot objects in our array and clears the array itself. */
  virtual void clear();

  /** Adds the passed slot to the end. */
  virtual void addSlot(PluginSlot *slotToAdd);
    // \todo generalize this function into an "insertSlot" function which also takes an index at 
    // which  position it should be inserted (with -1 as default which means "at the end"). 

  /** Deletes the slot with given index.*/
  virtual void deleteSlot(int index);

  /** Deletes the given slot object. */
  virtual void deleteSlot(PluginSlot *slotToDelete);

  //-----------------------------------------------------------------------------------------------
  // audio processing:   
    
  /** Applies the plugin chain to the passed buffers. */
  virtual void processBlock(const AudioSourceChannelInfo &bufferToFill, MidiBuffer &midiMessages);

protected:

  Array<PluginSlot*> pluginSlots;
    // maybe we should use an OwnedArray

  CriticalSection mutex;
    // later, maybe have a pointer-to-CriticalSection and share it with the embedding Track

  JUCE_LEAK_DETECTOR(PluginChain);
};

// \todo getStateAsXml/setStateFromXml

#endif
