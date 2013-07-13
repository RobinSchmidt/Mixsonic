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

void PluginSlot::loadPlugin(const PluginDescription* description)
{
  AudioPluginInstance* tmpInstance = getPluginInstanceFromDescription(description);
  if( tmpInstance != nullptr )
    setPlugin(tmpInstance, true);
  else
    jassertfalse;
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
 
void PluginSlot::processBlock(AudioSampleBuffer& buffer, MidiBuffer& midiMessages) const
{
  ScopedLock lock(*mutex);
  if( plugin != nullptr && !bypass )     
    plugin->processBlock(buffer, midiMessages);
}

// misc:

XmlElement* PluginSlot::getStateAsXml() const
{
  ScopedLock lock(*mutex);
  XmlElement* xmlState = new XmlElement(String("PLUGIN"));
  if( !isEmpty() )
  {
    // store an identifier string that can be used to identify the plugin inside a KnownPluginList 
    // (this works across different machines, i.e. with different installation paths for the 
    // plugin):
    PluginDescription description;
    plugin->fillInPluginDescription(description);
    xmlState->setAttribute("identifier", description.createIdentifierString());

    // we may also store more verbose information about the plugin as xml-child element for 
    // convenience. this is redundant - for later identification, the identifier string stored 
    // above would be sufficient:
    bool verbose = false; // conveniently switch additional info on/off at compile time
    if( verbose )
      xmlState->addChildElement(description.createXml());

    // store the state of the plugin
    MemoryBlock pluginState;
    plugin->getStateInformation(pluginState);
    xmlState->setAttribute("state", pluginState.toBase64Encoding());
  }
  return xmlState;
}

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

// inquiry:

bool PluginChain::isEmpty() const
{  
  ScopedLock lock(mutex);
  for(int i = 0; i < pluginSlots.size(); i++)
  {
    if( !pluginSlots[i]->isEmpty() )
      return false;
  }
  return true;
}

// audio processing:

void PluginChain::processBlock(const AudioSourceChannelInfo &bufferToFill, 
                               MidiBuffer &midiMessages) const
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

// misc:  

XmlElement* PluginChain::getStateAsXml() const
{
  ScopedLock lock(mutex);
  if( isEmpty() )
    return nullptr;
  else
  {
    XmlElement* xmlState = new XmlElement(String("PLUGIN_CHAIN"));
    for(int i = 0; i < pluginSlots.size(); i++)
      xmlState->addChildElement(pluginSlots[i]->getStateAsXml());
    return xmlState;
  }
}
