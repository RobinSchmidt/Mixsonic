#include "PluginChain.h"

// construction/destruction:

PluginSlot::PluginSlot(AudioPluginInstance *pluginToUse, CriticalSection *mutexToUse) 
: Lockable(mutexToUse)
{
  plugin       = pluginToUse;
  bypass       = false;
  knownPlugins = &mixsonicGlobals->knownPlugins;
}

PluginSlot::~PluginSlot()
{
  ScopedLock lock(*mutex);
  deleteUnderlyingPlugin();
}

// setup:

bool PluginSlot::loadPlugin(const File& pluginFile)
{
  AudioPluginInstance* tmpInstance = getVSTPluginInstanceFromFile(pluginFile);
  if( tmpInstance != nullptr )
  {
    setPlugin(tmpInstance, true);
    return true;
  }
  else
  {
    jassertfalse; // preliminary \todo show error message - but we should do this from the GUI
                  // using the return value of this function
    return false;
  }
}

bool PluginSlot::loadPlugin(const PluginDescription* description)
{
  AudioPluginInstance* tmpInstance = getPluginInstanceFromDescription(description);
  if( tmpInstance != nullptr )
  {
    setPlugin(tmpInstance, true);
    return true;
  }
  else
  {
    jassertfalse;
    return false;
  }
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

// move this function into the file for global functions - this shall be used to create the 
// identifier-string to be used for recall
String createPluginIdentifierString(const String& format, const String& name,  
                                    const String& fileOrIdentifier, int uid)
{
  return format + "-" + name + "-" + String::toHexString(fileOrIdentifier.hashCode())   
         + "-" + String::toHexString(uid);
}

XmlElement* PluginSlot::getStateAsXml() const
{
  ScopedLock lock(*mutex);
  XmlElement* xmlState = nullptr;
  if( !isEmpty() )
  {
    // store the info to identify the plugin:
    PluginDescription description;
    plugin->fillInPluginDescription(description);
    xmlState = description.createXml();

    // store the state of the plugin as additional attribute:
    MemoryBlock pluginState;
    plugin->getStateInformation(pluginState);
    xmlState->setAttribute("state", pluginState.toBase64Encoding());
  }
  return xmlState;

  /*
  XmlElement* xmlState = new XmlElement(String("PLUGIN"));
  if( !isEmpty() )
  {
    // store the information, required to identify the plugin later - actually, only name, format,
    // uid and fileOrIdentifier are required for identification, the rest is just for info:
    PluginDescription description;
    plugin->fillInPluginDescription(description);
    xmlState->setAttribute("name",             description.name);
    xmlState->setAttribute("manufacturer",     description.manufacturerName);
    xmlState->setAttribute("version",          description.version);
    xmlState->setAttribute("uid",              description.uid);
    xmlState->setAttribute("format",           description.pluginFormatName);
    //xmlState->setAttribute("numInputs",        description.numInputChannels);
    //xmlState->setAttribute("numOutputs",       description.numOutputChannels);
    if( description.pluginFormatName == "AU" )
      xmlState->setAttribute("identifier", description.fileOrIdentifier);
    else
      xmlState->setAttribute("path", description.fileOrIdentifier);

    // store the state of the plugin:
    MemoryBlock pluginState;
    plugin->getStateInformation(pluginState);
    xmlState->setAttribute("state", pluginState.toBase64Encoding());
  }
  return xmlState;
  */
}

void PluginSlot::setStateFromXml(const XmlElement& xmlState)
{
  int    uid      = xmlState.getIntAttribute("uid");
  String name     = xmlState.getStringAttribute("name");
  String format   = xmlState.getStringAttribute("format");
  String fileName = xmlState.getStringAttribute("file");

  String identifierString = createPluginIdentifierString(format, name, fileName, uid);

  /*
  String fileOrIdentifier;
  if( format == "AU" )
    fileOrIdentifier = xmlState.getStringAttribute("identifier");
  else
    fileOrIdentifier = xmlState.getStringAttribute("path");
    */


  // try to find the plugin in the list of known plugins, if it is found, load it:
  bool pluginLoaded = false;
  if( knownPlugins != nullptr )
  {
    PluginDescription *description = knownPlugins->getTypeForIdentifierString(identifierString);
    if( description != nullptr )
      pluginLoaded = loadPlugin(description);
  }

  // if the plugin was not in the list, try to load it based on the file-path:
  if( !pluginLoaded )
  {
    if( format == "AU" )
    {
      jassertfalse; // it's not an actual filename in case of AUs - not yet sure how to handle this case
    }
    else
      pluginLoaded = loadPlugin(File(fileName));
  }

  // if loading from the filename also failed, prompt the user to locate the file:
  if( !pluginLoaded )
  {
    jassertfalse; // here, we should open a filebrowser dialog where the user can locate the plugin
  }

  // if loading the plugin still failed (i.e. the user dismissed the filebrowser dialog), we put a 
  // dummy plugin into the slot which just stores the information about the plugin and its state, 
  // so we can save it again as part of the song:
  if( !pluginLoaded )
    insertDummyPlugin(xmlState);
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

void PluginSlot::insertDummyPlugin(const XmlElement& xmlState)
{
  jassertfalse; // this function is still under construction

  //DummyPlugin *dummy = new DummyPlugin();
  //dummy->setPluginDescriptionFromXml(xmlState);
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
