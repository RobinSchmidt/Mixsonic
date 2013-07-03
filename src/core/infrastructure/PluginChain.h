#ifndef PluginChain_h
#define PluginChain_h

//#include "../../libs/JuceIncludes.h"
#include "GlobalFunctions.h"


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

  bool isEmpty() const { return plugin == nullptr; }

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

class PluginChain /*: public ChangeBroadcaster*/
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

  /** Adds the passed slot to the end.
  \todo generalize this function into an "insertSlot" function which also takes an index at which
        position it should be inserted (with -1 as default which means "at the end").  */
  virtual void addSlot(PluginSlot *slotToAdd);

  /** Removes the slot with given index and optionally deletes the 
  AudioPluginInstance object.*/
  //virtual void removeSlot(int index, bool deletePluginInstance);

  /** Removes the given slot object and optionally deletes the AudioPluginInstance object. */
  //virtual void removeSlot(PluginSlot *slotToRemove, bool deleteSlot);

  virtual void deleteSlot(int index);

  virtual void deleteSlot(PluginSlot *slotToDelete);



      
  /** Inserts the passed plugin instance into the plugin slot with given index, optionally 
  deleting the old plugin instance. */
  //virtual void insertPlugin(int slotIndex, AudioPluginInstance* pluginToInsert, 
  //                          bool deleteOldPlugin = true);

  /** Inserts the passed plugin instance into the slot to which the passed pointer points. If this 
  slot is not yet element of our "pluginSlots" array, it will be appended */
  //virtual void insertPlugin(PluginSlot *slot, AudioPluginInstance* pluginToInsert, 
  //                          bool deleteOldPlugin = true);


  /** Removes a plugin from the slot with given index and optionally deletes the 
  AudioPluginInstance object. The slot it self will not be removed - it will remain as empty 
  slot. */
  //virtual void removePlugin(int slotIndex, bool deletePluginInstance);

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
    // maybe we should use an OwnedArray


  JUCE_LEAK_DETECTOR(PluginChain);
};

// \todo getStateAsXml/setStateFromXml

#endif
