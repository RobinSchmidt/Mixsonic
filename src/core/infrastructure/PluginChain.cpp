#include "PluginChain.h"

// construction/destruction:

PluginSlot::PluginSlot(AudioPluginInstance *pluginToUse, CriticalSection *mutexToUse) 
: Lockable(mutexToUse)
{
  plugin = pluginToUse;
  bypass = false;
}

PluginSlot::~PluginSlot()
{
  ScopedLock lock(*mutex);
  deleteUnderlyingPlugin();
}

// setup:

void PluginSlot::loadPlugin(const File& pluginFile)
{
  //ScopedLock lock(*mutex); // may be unecessary
  AudioPluginInstance* tmpInstance = getVSTPluginInstanceFromFile(pluginFile);
  if( tmpInstance != nullptr )
    setPlugin(tmpInstance, true);
  else
    jassertfalse; // preliminary \todo show error message
}

void PluginSlot::setPlugin(AudioPluginInstance* pluginToUse, bool deleteOldPlugin)
{
  ScopedLock lock(*mutex);
  if( deleteOldPlugin )
    deleteUnderlyingPlugin();
  plugin = pluginToUse;
  sendSynchronousChangeMessage(); 
}

void PluginSlot::setBypass(bool shouldBeBypassed) 
{ 
  ScopedLock lock(*mutex);
  bypass = shouldBeBypassed; 
} 

// audio processing:
 
void PluginSlot::processBlock(AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
  ScopedLock lock(*mutex);
  if( plugin != nullptr && !bypass )     
    plugin->processBlock(buffer, midiMessages);
}

// misc:

void PluginSlot::deleteUnderlyingPlugin()
{
  ScopedLock lock(*mutex);
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

PluginChain::PluginChain() : Lockable(&mutex)
{

}

PluginChain::~PluginChain()
{
  clear();
}

// setup:

void PluginChain::clear()
{
  enterLock();
  for(int i = 0; i < pluginSlots.size(); i++)
    delete pluginSlots[i];
  pluginSlots.clear();
  exitLock();
}

void PluginChain::addSlot(PluginSlot *slotToAdd)
{
  enterLock();
  pluginSlots.add(slotToAdd);
  exitLock();
}

void PluginChain::deleteSlot(int index)
{
  enterLock();
  jassert( index >= 0 && index < pluginSlots.size() );
  delete pluginSlots[index];
  pluginSlots.remove(index);
  exitLock();
}

void PluginChain::deleteSlot(PluginSlot *slotToDelete)
{
  enterLock();
  jassert(pluginSlots.contains(slotToDelete));
  pluginSlots.removeValue(slotToDelete);
  delete slotToDelete;
  exitLock();
}

void PluginChain::processBlock(const AudioSourceChannelInfo &bufferToFill, 
                               MidiBuffer &midiMessages)
{
  // modify pointers in tmpBuffer to take into accout a possibly nonzero startSample value in 
  // bufferToFill:
  float **channelArray = bufferToFill.buffer->getArrayOfChannels();
  for(int i = 0; i < bufferToFill.buffer->getNumChannels(); i++)
    channelArray[i] += bufferToFill.startSample;

  // loop over slots to apply all plugins:
  enterLock();
  for(int i = 0; i < pluginSlots.size(); i++)
    pluginSlots[i]->processBlock(*bufferToFill.buffer, midiMessages);
  exitLock();

  // undo startsample offset:
  for(int i = 0; i < bufferToFill.buffer->getNumChannels(); i++)
    channelArray[i] -= bufferToFill.startSample;
}
