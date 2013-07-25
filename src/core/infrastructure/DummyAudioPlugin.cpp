#include "DummyAudioPlugin.h"
  
void DummyAudioPlugin::setPluginDescriptionFromXml(const XmlElement& xml)
{
  description.name              = xml.getStringAttribute("name");
  description.descriptiveName   = xml.getStringAttribute("descriptiveName", description.name);
  description.pluginFormatName  = xml.getStringAttribute("format");
  description.category          = xml.getStringAttribute("category");
  description.manufacturerName  = xml.getStringAttribute("manufacturer");
  description.version           = xml.getStringAttribute("version");
  description.fileOrIdentifier  = xml.getStringAttribute("file");
  description.uid               = xml.getIntAttribute("uid");
  description.isInstrument      = xml.getBoolAttribute("isInstrument");
  description.numInputChannels  = xml.getIntAttribute("numInputs");
  description.numOutputChannels = xml.getIntAttribute("numOutputs");
}

const String DummyAudioPlugin::getFormatName() const
{
  return description.pluginFormatName;
}

const String DummyAudioPlugin::getPluginName() const
{
  return description.name;
}

const String DummyAudioPlugin::getManufacturerName() const
{
  return description.manufacturerName;
}

const String DummyAudioPlugin::getVersion() const
{
  return description.version;
}

const int DummyAudioPlugin::getUniqueID() const
{
  return description.uid;
}

void DummyAudioPlugin::fillInPluginDescription(PluginDescription& d) const
{
  d.category          = description.category;
  d.descriptiveName   = description.descriptiveName;
  d.fileOrIdentifier  = description.fileOrIdentifier;
  d.isInstrument      = description.isInstrument;
  d.lastFileModTime   = description.lastFileModTime;
  d.manufacturerName  = description.manufacturerName;
  d.name              = description.name;
  d.numInputChannels  = description.numInputChannels;
  d.numOutputChannels = description.numOutputChannels;
  d.pluginFormatName  = description.pluginFormatName;
  d.uid               = description.uid;
  d.version           = description.version;
}

void DummyAudioPlugin::getStateInformation(MemoryBlock &destData)
{
  int byteSize = state.getSize();
  destData.setSize(byteSize);
  destData.copyFrom(state.getData(), 0, byteSize);
}
 
void DummyAudioPlugin::setStateInformation(const void *data, int sizeInBytes)
{
  state.setSize(sizeInBytes);
  state.copyFrom(data, 0, sizeInBytes);
}
 
const String DummyAudioPlugin::getName() const
{
  return String("Error: ") + description.name;
}
 
void DummyAudioPlugin::prepareToPlay(double sampleRate, int estimatedSamplesPerBlock)
{

}
 
void DummyAudioPlugin::releaseResources() 
{

}

void DummyAudioPlugin::processBlock(AudioSampleBuffer &buffer, MidiBuffer &midiMessages)
{

}

const String DummyAudioPlugin::getInputChannelName(int channelIndex) const
{
  return String("Input ") + String(channelIndex);
}
 
const String DummyAudioPlugin::getOutputChannelName(int channelIndex) const
{
  return String("Output ") + String(channelIndex);
}
 
bool DummyAudioPlugin::isInputChannelStereoPair(int index) const
{
  return true;
}
  
bool DummyAudioPlugin::isOutputChannelStereoPair(int index) const
{
  return true;
}

bool DummyAudioPlugin::acceptsMidi() const
{
  return true;
}

bool DummyAudioPlugin::producesMidi() const
{
  return true;
}
 
AudioProcessorEditor* DummyAudioPlugin::createEditor()
{
  return new DummyAudioPluginEditor(this);
}
 
bool DummyAudioPlugin::hasEditor() const
{
  return true;
}
 
int DummyAudioPlugin::getNumParameters()
{
  return 0;
}
 
const String DummyAudioPlugin::getParameterName(int parameterIndex)
{
  return String::empty;
}
 
float DummyAudioPlugin::getParameter(int parameterIndex)
{
  return 0.f;
}
 
const String DummyAudioPlugin::getParameterText(int parameterIndex)
{
  return String::empty;
}
 
void DummyAudioPlugin::setParameter(int parameterIndex, float newValue)
{

}
 
int DummyAudioPlugin::getNumPrograms()
{
  return 0;
}
 
int DummyAudioPlugin::getCurrentProgram()
{
  return 0;
}
 
void DummyAudioPlugin::setCurrentProgram(int index)
{

}
 
const String DummyAudioPlugin::getProgramName(int index)
{
  return String::empty;
}
 
void DummyAudioPlugin::changeProgramName(int index, const String& newName)
{

}

//=================================================================================================

DummyAudioPluginEditor::DummyAudioPluginEditor(AudioProcessor *owner) : AudioProcessorEditor(owner)
{
  dummyPlugin = dynamic_cast<DummyAudioPlugin*> (owner);
  jassert(dummyPlugin != nullptr);

  addAndMakeVisible(formatLabel = new RLabel);
  formatLabel->setText(dummyPlugin->getFormatName() + String(" Plugin is unavailable."), false);

  addAndMakeVisible(nameLabel = new RLabel);
  nameLabel->setText(String("Plugin Name: ") + dummyPlugin->getPluginName(), false);

  addAndMakeVisible(manufacturerLabel = new RLabel);
  manufacturerLabel->setText(String("Manufacturer: ") + dummyPlugin->getManufacturerName(), false);

  addAndMakeVisible(versionLabel = new RLabel);
  versionLabel->setText(String("Version: ") + dummyPlugin->getVersion(), false);

  addAndMakeVisible(uidLabel = new RLabel);
  uidLabel->setText(String("Unique ID: ") + String(dummyPlugin->getUniqueID()), false);

  setSize(300, numLabels*labelHeight);
}

DummyAudioPluginEditor::~DummyAudioPluginEditor()
{
  deleteAllChildren();

  dummyPlugin->editorBeingDeleted(this);
    // if we don't do this, juce triggers a breakpoint in 
    // AudioProcessorEditor::~AudioProcessorEditor() when closing the app (if we have opened an 
    // editor before)
}

void DummyAudioPluginEditor::resized()
{
  int x = 0;
  int y = 0;
  int w = getWidth();
  int h = labelHeight;

  formatLabel->setBounds(x, y, w, h);
  y += h;
  nameLabel->setBounds(x, y, w, h);
  y += h;
  manufacturerLabel->setBounds(x, y, w, h);
  y += h;
  versionLabel->setBounds(x, y, w, h);
  y += h;
  uidLabel->setBounds(x, y, w, h);
}
