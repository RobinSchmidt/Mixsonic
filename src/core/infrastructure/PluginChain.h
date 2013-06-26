#ifndef PluginChain_h
#define PluginChain_h

#include "../../libs/JuceIncludes.h"


/**

This is a class for representing a slot for plugins which may (or may not) contain an 
AudioPluginInstance and handles some additional settings such as bypass, etc.

\todo: perhaps, we need a PluginSlotObserver class and derive PluginSlotComponent from it

*/

class PluginSlot : public ChangeBroadcaster
{  
  
  friend class PluginChain;              // needs direct access to "plugin" member
  friend class AudioPluginSlotComponent; // ditto

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  PluginSlot(AudioPluginInstance *pluginToUse = nullptr);  

  /** Destructor. */
  ~PluginSlot();           

  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Loads a plugin specified by the plugin's shared library file into this slot. If the library 
  can't be loaded as plugin, nothing will be done and an error message is shown. */
  void loadPlugin(const File& pluginFile);

  /** Assigns a new plugin (instance) to this slot and optionally deletes the old one. */
  void setPlugin(AudioPluginInstance* pluginToUse, bool deleteOldPlugin);

  /** Switches bypass mode on/off */
  void setBypass(bool shouldBeBypassed) { bypass = shouldBeBypassed; } 

  //-----------------------------------------------------------------------------------------------
  // inquiry:

  bool isBypassed() const { return bypass; }

protected:

  /** Deletes the plugin instance that is used in this slot, thereby making sure that no editor
  is open. It also resets our pointer to null. */
  void deleteUnderlyingPlugin();

  AudioPluginInstance *plugin;

  bool bypass;  // maybe, we can replace this with a continuous dry/wet control (0.0 corresponds
                // to a bypassed setting)

  JUCE_LEAK_DETECTOR(PluginSlot);
};

//=================================================================================================

/**

This is a class for representing chain (i.e. series connection) of plugins. 

*/

class PluginChain : public ChangeBroadcaster
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

  /** Adds an empty slot to the end of this chain. */
  virtual void addEmptySlot();

  /** Removes the slot with given index and optionally deletes the 
  AudioPluginInstance object.*/
  virtual void removeSlot(int index, bool deletePluginInstance);

  /** Inserts the passed plugin instance into the plugin slot with given index, optionally 
  deleting the old plugin instance. */
  virtual void insertPlugin(int slotIndex, AudioPluginInstance* pluginToInsert, 
                            bool deleteOldPlugin = true);

  /** Removes a plugin from the slot with given index and optionally deletes the 
  AudioPluginInstance object. The slot it self will not be removed - it will remain as empty 
  slot. */
  virtual void removePlugin(int slotIndex, bool deletePluginInstance);

  //-----------------------------------------------------------------------------------------------
  // inquiry:

  /** Returns a pointer to the AudioPluginInstance at the given slot index. */
  //AudioPluginInstance* getPlugin(int slotIndex);

  //-----------------------------------------------------------------------------------------------
  // audio processing:   
    
  /** Applies the plugin chain to the passed buffers. */
  virtual void processBlock(const AudioSourceChannelInfo &bufferToFill, 
                            MidiBuffer &midiMessages) const;


  //===============================================================================================
  juce_UseDebuggingNewOperator;

protected:

  Array<PluginSlot*, CriticalSection> pluginSlots;


  JUCE_LEAK_DETECTOR(PluginChain);
};

// \todo getStateAsXml/setStateFromXml

#endif
