#include "PluginChain.h"

// construction/destruction:

PluginSlot::PluginSlot(AudioPluginInstance *pluginToUse) 
{
  plugin = pluginToUse;
  bypass = false;
}

PluginSlot::~PluginSlot()
{
  deleteUnderlyingPlugin();
}

// setup:

void PluginSlot::loadPlugin(const File& pluginFile)
{
  AudioPluginInstance* tmpInstance = getVSTPluginInstanceFromFile(pluginFile);
  if( tmpInstance != nullptr )
    setPlugin(tmpInstance, true);
  else
    jassertfalse; // preliminary \todo show error message
}

void PluginSlot::setPlugin(AudioPluginInstance* pluginToUse, bool deleteOldPlugin)
{
  if( deleteOldPlugin )
    deleteUnderlyingPlugin();
  plugin = pluginToUse;
  sendSynchronousChangeMessage();
  //sendChangeMessage();
}

// misc:

void PluginSlot::deleteUnderlyingPlugin()
{
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


  int i;
  //AudioSampleBuffer tmpBuffer(*bufferToFill.buffer);

  // modify the pointers in the tmpBuffer to take into accout a possibly nonzero
  // startSample value in bufferToFill:
  float **channelArray = bufferToFill.buffer->getArrayOfChannels();
  for(i = 0; i < bufferToFill.buffer->getNumChannels(); i++)
    channelArray[i] += bufferToFill.startSample;

  // loop over the slots to apply all the plugins:
  for(i = 0; i < pluginSlots.size(); i++)
  {
    if( pluginSlots[i]->plugin != nullptr && !pluginSlots[i]->bypass )
      pluginSlots[i]->plugin->processBlock(*bufferToFill.buffer, midiMessages);
  }

  // undo the startsample offset:
  for(i = 0; i < bufferToFill.buffer->getNumChannels(); i++)
    channelArray[i] -= bufferToFill.startSample;



  /*
  // doesn't work - the copy-constructor for AudioSampleBuffer creates a deep copy:
  int i;
  AudioSampleBuffer tmpBuffer(*bufferToFill.buffer);

  // modify the pointers in the tmpBuffer to take into accout a possibly nonzero
  // startSample value in bufferToFill:
  float **channelArray = tmpBuffer.getArrayOfChannels();
  for(i = 0; i < tmpBuffer.getNumChannels(); i++)
    channelArray[i] += bufferToFill.startSample;

  // loop over the slots to apply all the plugins:
  for(i = 0; i < pluginSlots.size(); i++)
  {
    if( pluginSlots[i]->plugin != nullptr && !pluginSlots[i]->bypass )
      pluginSlots[i]->plugin->processBlock(tmpBuffer, midiMessages);
  }

  // undo the startsample offset:
  for(i = 0; i < tmpBuffer.getNumChannels(); i++)
    channelArray[i] -= bufferToFill.startSample;
    */
}
