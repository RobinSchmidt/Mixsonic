#ifndef MixsonicAudioRecordDialog_h
#define MixsonicAudioRecordDialog_h

#include "MixsonicAudioFileSettingsDialog.h"
#include "../../core/audio/AudioRecorder.h"

/**

This class the dialog for recording an audiofile from the audiointerface.

*/

class MixsonicAudioRecordDialog : public MixsonicAudioFileSettingsDialog, 
  public AudioIODeviceCallback
{

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  MixsonicAudioRecordDialog(AudioDeviceManager* deviceManagerToUse, 
    const File& defaultTargetFile = File::nonexistent);

  /** Destructor. */
  virtual ~MixsonicAudioRecordDialog();

  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Overrides setDescriptionField(). */
  virtual void setDescriptionField(Label* newDescriptionField);

  //-----------------------------------------------------------------------------------------------
  // callbacks:

  /** Overrides buttonClicked(). */
  virtual void buttonClicked(Button *buttonThatWasClicked);

  /** Overrides resized(). */
  virtual void resized();

  /** Overrides audioDeviceIOCallback(). */
  virtual void audioDeviceIOCallback(const float **inputChannelData, int numInputChannels, 
    float **outputChannelData, int numOutputChannels, int numSamples); 
 
  /** Overrides audioDeviceAboutToStart(). */
  virtual void audioDeviceAboutToStart(AudioIODevice *device);
 
  /** Overrides audioDeviceStopped(). */
  virtual void audioDeviceStopped();

  //===============================================================================================
  juce_UseDebuggingNewOperator;
  
protected:

  /** Starts the recording process. */
  virtual void startRecording();

  /** Starts the recording process. */
  virtual void stopRecording();

  // the widgets specific to this dialog:
  RButton *recordButton;

  // stuff for the actual recording:
  AudioRecorder       recorder;
  AudioDeviceManager* deviceManager;

};

#endif	
