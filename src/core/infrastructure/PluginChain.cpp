#include "PluginChain.h"

// construction/destruction:

PluginSlot::PluginSlot(AudioPluginInstance *pluginToUse) 
{
  plugin = pluginToUse;
}

PluginSlot::~PluginSlot()
{
  deleteUnderlyingPlugin();
}

// setup:

void PluginSlot::loadPlugin(const File& pluginFile)
{


  int dummy = 0;
}

void PluginSlot::setPlugin(AudioPluginInstance* pluginToUse, bool deleteOldPlugin)
{
  if( deleteOldPlugin )
    deleteUnderlyingPlugin();
  plugin = pluginToUse;
  sendChangeMessage();
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
  sendChangeMessage();
}

void PluginChain::removeSlot(int index, bool deletePluginInstance)
{
  jassertfalse; // not yet implemented
}

void PluginChain::insertPlugin(int slotIndex, AudioPluginInstance* pluginToInsert,
                               bool deleteOldPlugin)
{
  ScopedLock lock(pluginSlots.getLock());
  jassert( slotIndex >= 0 && slotIndex < pluginSlots.size() );
  pluginSlots[slotIndex]->setPlugin(pluginToInsert, deleteOldPlugin);
  sendChangeMessage();
}

void PluginChain::removePlugin(int slotIndex, bool deletePluginInstance)
{
  insertPlugin(slotIndex, nullptr, deletePluginInstance);
}

void PluginChain::processBlock(const AudioSourceChannelInfo &bufferToFill, 
                               MidiBuffer &midiMessages) const
{
  ScopedLock lock(pluginSlots.getLock());

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
}
