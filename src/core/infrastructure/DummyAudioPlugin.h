#ifndef DummyAudioPlugin_h
#define DummyAudioPlugin_h

#include "../../components/widgets/RLabel.h"
#include "../../components/reusable/UserInterfaceSection.h"

/** A dummy plugin class that is used when a plugin can't be found when a stored state of a 
PluginSlot is restored via PluginSlot::setStateFromXml. The purpose of the dummy plugin is to
serve as placeholder and store the identification and state information for the unavailable 
plugin. 

maybe rename to PlaceHolderAudioPlugin

*/

class DummyAudioPlugin : public AudioPluginInstance
{
public:

  /** Sets up our PluginDescription member from an XmlElement. This description member is used to
  store information to indentify the plugin. */
  virtual void setPluginDescriptionFromXml(const XmlElement& xml);

  /** Returns the format name of the plugin. */
  virtual const String getFormatName() const;

  /** Returns the name of the plugin without a prepended "Error: ". The overriden getName function
  instead prepends this error-notification, so we can see it in the PluginSlotComponent. */
  virtual const String getPluginName() const;

  /** Returns the name of the manufacturer of the plugin. */
  virtual const String getManufacturerName() const;

  /** Returns a string representing the version of the plugin. */
  virtual const String getVersion() const;

  /** Returns a string representing the version of the plugin. */
  virtual const int getUniqueID() const;


  // overrides for AudioPluginInstance baseclass:
  virtual void fillInPluginDescription(PluginDescription& description) const;

  // overrides for (indirect) AudioProcessor baseclass:
  virtual void getStateInformation(MemoryBlock &destData);
  virtual void setStateInformation(const void *data, int sizeInBytes);
  virtual const String getName() const;
  virtual void prepareToPlay(double sampleRate, int estimatedSamplesPerBlock);
  virtual void releaseResources();
  virtual void processBlock(AudioSampleBuffer &buffer, MidiBuffer &midiMessages);
  virtual const String getInputChannelName(int channelIndex) const;
  virtual const String getOutputChannelName(int channelIndex) const;
  virtual bool isInputChannelStereoPair(int index) const;
  virtual bool isOutputChannelStereoPair(int index) const;
  virtual bool acceptsMidi() const;
  virtual bool producesMidi() const;
  virtual AudioProcessorEditor* createEditor();  
  virtual bool hasEditor() const;
  virtual int getNumParameters();
  virtual const String getParameterName(int parameterIndex);
  virtual float getParameter(int parameterIndex);
  virtual const String getParameterText(int parameterIndex);
  virtual void setParameter(int parameterIndex, float newValue);
  virtual int getNumPrograms();
  virtual int getCurrentProgram();
  virtual void setCurrentProgram(int index);
  virtual const String getProgramName(int index);
  virtual void changeProgramName(int index, const String& newName);

protected:

  PluginDescription description;
  MemoryBlock state;

  JUCE_LEAK_DETECTOR(DummyAudioPlugin);
};

//=================================================================================================

/** An "editor" class for the DummyAudioPlugin class. Actually, it just shows some information 
about the plugin - there's nothing to edit. */

class DummyAudioPluginEditor : public AudioProcessorEditor, public UserInterfaceSection
{

public:

  DummyAudioPluginEditor(AudioProcessor *owner);

  virtual ~DummyAudioPluginEditor();

  virtual void resized();

protected:

  RLabel *formatLabel, *nameLabel, *manufacturerLabel, *versionLabel, *uidLabel;
  static const int numLabels   = 5;
  static const int labelHeight = 20;

  DummyAudioPlugin *dummyPlugin;

  JUCE_LEAK_DETECTOR(DummyAudioPluginEditor);
};

#endif
