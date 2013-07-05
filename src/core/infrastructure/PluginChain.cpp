#include "PluginChain.h"

// construction/destruction:

PluginSlot::PluginSlot(AudioPluginInstance *pluginToUse) 
{
  ScopedLock lock(pluginLock);
  plugin = pluginToUse;
  bypass = false;
}

PluginSlot::~PluginSlot()
{
  ScopedLock lock(pluginLock);
  deleteUnderlyingPlugin();
}

// setup:

void PluginSlot::loadPlugin(const File& pluginFile)
{
  ScopedLock lock(pluginLock);
  AudioPluginInstance* tmpInstance = getVSTPluginInstanceFromFile(pluginFile);
  if( tmpInstance != nullptr )
    setPlugin(tmpInstance, true);
  else
    jassertfalse; // preliminary \todo show error message
}

void PluginSlot::setPlugin(AudioPluginInstance* pluginToUse, bool deleteOldPlugin)
{
  ScopedLock lock(pluginLock);
  if( deleteOldPlugin )
    deleteUnderlyingPlugin();
  plugin = pluginToUse;

  //sendChangeMessage();
  sendSynchronousChangeMessage(); 
    // may cause a deadlock, but that's fortunate because it points to some general problem with
    // the locking strategy
}

// audio processing:
 
void PluginSlot::processBlock(AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
  ScopedLock lock(pluginLock);
  if( plugin != nullptr && !bypass )     
    plugin->processBlock(buffer, midiMessages);
}

// misc:

void PluginSlot::deleteUnderlyingPlugin()
{
  ScopedLock lock(pluginLock);
  if( plugin == nullptr )
    return;

  // make sure that no editor is open:
  AudioProcessorEditor *editor = plugin->getActiveEditor();
  if( editor != nullptr )
    delete editor;

  // can there be other objects that still hold pointers to the plugin object? if so, we should 
  // make sure here, that these nullify their pointers

  delete plugin;
  plugin = nullptr;
}

//=================================================================================================

// construction/destruction:

PluginChain::PluginChain()
{

}

PluginChain::~PluginChain()
{
  clear();
}

// setup:

void PluginChain::clear()
{
  ScopedLock lock(pluginSlots.getLock());
  for(int i = 0; i < pluginSlots.size(); i++)
    delete pluginSlots[i];
  pluginSlots.clear();
}

void PluginChain::addEmptySlot()
{
  ScopedLock lock(pluginSlots.getLock());
  pluginSlots.add(new PluginSlot);
  //sendSynchronousChangeMessage();
}

void PluginChain::addSlot(PluginSlot *slotToAdd)
{
  ScopedLock lock(pluginSlots.getLock());
  pluginSlots.add(slotToAdd);
  //sendSynchronousChangeMessage();
}

/*
void PluginChain::removeSlot(int index, bool deletePluginInstance)
{
  jassertfalse; // not yet implemented
}

void PluginChain::removeSlot(PluginSlot *slotToRemove, bool deletePluginInstance)
{
  ScopedLock lock(pluginSlots.getLock());
  jassert(pluginSlots.contains(slotToRemove));
  pluginSlots.removeValue(slotToRemove);
}
*/

void PluginChain::deleteSlot(int index)
{
  ScopedLock lock(pluginSlots.getLock());
  jassert( index >= 0 && index < pluginSlots.size() );
  delete pluginSlots[index];
  pluginSlots.remove(index);
}

void PluginChain::deleteSlot(PluginSlot *slotToDelete)
{
  ScopedLock lock(pluginSlots.getLock());
  jassert(pluginSlots.contains(slotToDelete));
  pluginSlots.removeValue(slotToDelete);
  delete slotToDelete;
}

/*
void PluginChain::insertPlugin(int slotIndex, AudioPluginInstance* pluginToInsert,
                               bool deleteOldPlugin)
{
  ScopedLock lock(pluginSlots.getLock());
  jassert( slotIndex >= 0 && slotIndex < pluginSlots.size() );
  pluginSlots[slotIndex]->setPlugin(pluginToInsert, deleteOldPlugin);
  sendSynchronousChangeMessage();
}

void PluginChain::removePlugin(int slotIndex, bool deletePluginInstance)
{
  insertPlugin(slotIndex, nullptr, deletePluginInstance);
}
*/

void PluginChain::processBlock(const AudioSourceChannelInfo &bufferToFill, 
                               MidiBuffer &midiMessages) const
{
  ScopedLock lock(pluginSlots.getLock());

  // modify pointers in tmpBuffer to take into accout a possibly nonzero startSample value in 
  // bufferToFill:
  float **channelArray = bufferToFill.buffer->getArrayOfChannels();
  for(int i = 0; i < bufferToFill.buffer->getNumChannels(); i++)
    channelArray[i] += bufferToFill.startSample;

  // loop over slots to apply all plugins:
  for(int i = 0; i < pluginSlots.size(); i++)
  {
    pluginSlots[i]->processBlock(*bufferToFill.buffer, midiMessages);
    //if( pluginSlots[i]->plugin != nullptr && !pluginSlots[i]->bypass )
    //  pluginSlots[i]->plugin->processBlock(*bufferToFill.buffer, midiMessages);
  }

  // undo startsample offset:
  for(int i = 0; i < bufferToFill.buffer->getNumChannels(); i++)
    channelArray[i] -= bufferToFill.startSample;
}
