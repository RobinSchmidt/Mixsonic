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
  if( pluginFile == File::nonexistent )
    return false;

  AudioPluginInstance* tmpInstance = getVSTPluginInstanceFromFile(pluginFile);
  if( tmpInstance != nullptr )
  {
    setPlugin(tmpInstance, true);
    return true;
  }
  else
    return false;
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
  enterLock();
  processBlockUnsafe(buffer, midiMessages);
  exitLock();
}

void PluginSlot::processBlockUnsafe(AudioSampleBuffer& buffer, MidiBuffer& midiMessages) const
{
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
    // store info to identify plugin:
    xmlState = new XmlElement ("PLUGIN");
    PluginDescription description;
    plugin->fillInPluginDescription(description);
    xmlState->setAttribute("name", description.name);
    if(description.descriptiveName != description.name)
        xmlState->setAttribute("descriptiveName", description.descriptiveName);
    xmlState->setAttribute("format", description.pluginFormatName);
    xmlState->setAttribute("category", description.category);
    xmlState->setAttribute("manufacturer", description.manufacturerName);
    xmlState->setAttribute("version", description.version);
    xmlState->setAttribute("file", description.fileOrIdentifier);
    xmlState->setAttribute("uid", description.uid);
    xmlState->setAttribute("isInstrument", description.isInstrument);
    xmlState->setAttribute("numInputs", description.numInputChannels);
    xmlState->setAttribute("numOutputs", description.numOutputChannels);

    // store bypass-setting, if toggled:
    if( isBypassed() )
      xmlState->setAttribute("bypass", true);

    // store state of the plugin:
    MemoryBlock pluginState;
    plugin->getStateInformation(pluginState);
    xmlState->setAttribute("state", pluginState.toBase64Encoding());
  }
  return xmlState;
}

void PluginSlot::setStateFromXml(const XmlElement& xmlState)
{
  int    uid          = xmlState.getIntAttribute("uid");
  String name         = xmlState.getStringAttribute("name");
  String format       = xmlState.getStringAttribute("format");
  String fileName     = xmlState.getStringAttribute("file");
  String manufacturer = xmlState.getStringAttribute("manufacturer");

  String identifierString = createPluginIdentifierString(format, name, fileName, uid);
  
  ScopedLock lock(*mutex);

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
    {
      pluginLoaded = loadPlugin(File(fileName));
        // \todo maybe open a dialog, where the user can add the plugin's parent directory to the 
        // plugin search path
    }
  }

  // if loading from the filename also failed, prompt the user to locate the file:
  if( !pluginLoaded )
  {
    if( showPluginManualSearchAlert(format, name, manufacturer, fileName) == true )
    {
      while( true )
      {
        File pluginFile = openLoadAudioPluginDialog();
        if( pluginFile == File::nonexistent ) 
          break; // user dimissed filechooser
        else if( doesPluginFileHaveUniqueId(pluginFile, uid) )
        {
          pluginLoaded = loadPlugin(pluginFile);
          break;
        }
        else
        {
          bool tryAgain = showWrongPluginFileAlert(format, name, manufacturer, 
            pluginFile.getFullPathName());
          if( tryAgain == false )
            break;
        }
      }
    }
  }

  // if loading the plugin still failed (i.e. the user dismissed the filebrowser dialog), we put a 
  // dummy plugin into the slot which just stores the information about the plugin and its state, 
  // so we can save it again as part of the song:
  if( !pluginLoaded )
    insertDummyPlugin(xmlState);

  // restore bypass setting:
  setBypass(xmlState.getBoolAttribute("bypass", false));

  // now, we have either inserted the right plugin or else a dummy plugin - now set up the plugin's
  // state:
  setPluginStateFromBase64Encoding(xmlState.getStringAttribute("state"));
}

void PluginSlot::setPluginStateFromBase64Encoding(const String& stateString)
{
  ScopedLock lock(*mutex);
  MemoryBlock block;
  bool success = block.fromBase64Encoding(stateString);
  if( success == true )
    setPluginState(block);
  else
    jassertfalse; // stateString corrupted - open message box
}

void PluginSlot::setPluginState(const MemoryBlock& state)
{
  ScopedLock lock(*mutex);
  if( plugin != nullptr )
    plugin->setStateInformation(state.getData(), state.getSize());
  else
    jassertfalse;
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
  DummyAudioPlugin *dummy = new DummyAudioPlugin();
  dummy->setPluginDescriptionFromXml(xmlState);
  setPlugin(dummy, true);
}

bool PluginSlot::showPluginManualSearchAlert(const String& format, const String& name, 
  const String& manufacturer, const String& path)
{
  String message;
  message += "The " + format + " plugin by" + manufacturer + 
    "could not be found in the standard plugin directory or in " + path + 
    ". Do you want to locate it manually?";
  return AlertWindow::showOkCancelBox(AlertWindow::WarningIcon, "Plugin not found!", message, 
    "Yes", "No");
}

bool PluginSlot::showWrongPluginFileAlert(const String& format, const String& name, 
    const String& manufacturer, const String& path)
{
  String message;
  message += "The file " + path + " is not the right file for the " + format + " plugin " 
    + name + " by " + manufacturer + ". Do you want to choose another file?";
  return AlertWindow::showOkCancelBox(AlertWindow::WarningIcon, "Wrong plugin file!", message, 
    "Yes", "No");
}

bool PluginSlot::doesPluginFileHaveUniqueId(const File& pluginFile, int uid)
{
  AudioPluginInstance* tmpInstance = getVSTPluginInstanceFromFile(pluginFile);
  if( tmpInstance == nullptr )
    return false;
  else
  {
    PluginDescription description;
    tmpInstance->fillInPluginDescription(description);
    delete tmpInstance;
    if( uid == description.uid  )
      return true;
    else
      return false;
  }
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
    pluginSlots[i]->processBlockUnsafe(*bufferToFill.buffer, midiMessages);
  exitLock();

  // undo startsample offset:
  for(int i = 0; i < bufferToFill.buffer->getNumChannels(); i++)
    channelArray[i] -= bufferToFill.startSample;

  // \todo: later, when we have one thread per track, we should do this offset stuff only once
  // in the audio callback of Arrangement. the tracks should always receive/fill buffers with
  // zero offset
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

void PluginChain::setStateFromXml(const XmlElement& xmlState)
{
  ScopedLock lock(mutex);
  clear();  
  PluginSlot *slot;
  forEachXmlChildElementWithTagName(xmlState, pluginState, "PLUGIN")
  {
    slot = new PluginSlot(nullptr, &mutex);
    slot->setStateFromXml(*pluginState);
    addSlot(slot);
  }
}
