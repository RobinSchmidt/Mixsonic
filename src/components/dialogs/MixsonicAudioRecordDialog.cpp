#include "MixsonicAudioRecordDialog.h"

MixsonicAudioRecordDialog::MixsonicAudioRecordDialog(SectionSkin *skinToUse, 
  AudioDeviceManager* deviceManagerToUse, const File& defaultTargetFile)
: MixsonicAudioFileSettingsDialog(skinToUse, defaultTargetFile)
{
  setHeadline(recordHeadlineStr);
  deviceManager = deviceManagerToUse;

  // create the additional button:
  recordButton = new MButton(&skin->widgetSkin, recordButtonStr);
  addAndMakeVisible(recordButton);
  recordButton->setDescription(recordButtonHelpStr);
  recordButton->setClickingTogglesState(true);
  recordButton->addListener(this);

  // make some inherited components invisible that are not used here:
  browseButton->setVisible(false);
  targetDirectoryLabel->setVisible(false);
  targetDirectoryField->setVisible(false);

  // initialize the target file:
  File targetFile = File(getApplicationDirectoryAsString() + String("/RecordedAudio.wav") );
  if( defaultTargetFile != File::nonexistent )
    targetFile = defaultTargetFile;
  textEntryField->setText(targetFile.getFileNameWithoutExtension() , false);
  updateAudioFileInfoAccordingToWidgets();

  // setup and disable the samplerate combo box - the recording sample rate is determined by the 
  // settings of the currently used audio device which is not in our control:
  sampleRateComboBox->setText(
    String(deviceManager->getCurrentAudioDevice()->getCurrentSampleRate()));
  sampleRateComboBox->setEnabled(false);
  bitDepthComboBox->setText( String(deviceManager->getCurrentAudioDevice()->getCurrentBitDepth()));
  bitDepthComboBox->setEnabled(false);

  setSize(700, 50);
}

MixsonicAudioRecordDialog::~MixsonicAudioRecordDialog()
{
  deleteAllChildren();
}

//-------------------------------------------------------------------------------------------------
// setup:

void MixsonicAudioRecordDialog::setDescriptionField(Label* newDescriptionField)
{
  MixsonicAudioFileSettingsDialog::setDescriptionField(newDescriptionField);
  recordButton->setDescriptionField(newDescriptionField);
}

//-------------------------------------------------------------------------------------------------
// callbacks:

void MixsonicAudioRecordDialog::buttonClicked(Button *buttonThatWasClicked)
{
  if( buttonThatWasClicked == recordButton )  
  {
    if( recordButton->getToggleState() == true )
      startRecording();
    else
      stopRecording();
  }
  else
    MixsonicAudioFileSettingsDialog::buttonClicked(buttonThatWasClicked);
}

void MixsonicAudioRecordDialog::resized()
{
  MixsonicModalDialog::resized();

  int x = 0;
  int y = headlineLabel->getBottom();
  int w = getWidth();
  //int h = getHeight();

  textEntryLabel->setBounds(x+4,  y+4, 120,  20);
  exitButton->setBounds(    w-62, y+4,  56, 20);  
  x = textEntryLabel->getRight();
  w = exitButton->getX()-x;
  textEntryField->setBounds(x+4, y+4, w-8, 20);

  recordButton->setBounds(exitButton->getX()-76, exitButton->getY(), 72, 20);
  x = textEntryLabel->getRight();
  w = recordButton->getX()-x;
  textEntryField->setBounds(x+4, exitButton->getY(), w-8, 20);
}

void MixsonicAudioRecordDialog::audioDeviceIOCallback(const float **inputChannelData, 
                                                      int numInputChannels,    
                                                      float **outputChannelData, 
                                                      int numOutputChannels, 
                                                      int numSamples)
{
  if( recorder.isRecording() )
    recorder.recordAudioBlock(inputChannelData, numSamples, numInputChannels);
}

void MixsonicAudioRecordDialog::audioDeviceAboutToStart(AudioIODevice *device)
{

}

void MixsonicAudioRecordDialog::audioDeviceStopped()
{

}

//-------------------------------------------------------------------------------------------------
// internal functions:

void MixsonicAudioRecordDialog::startRecording()
{
  File targetFile = fileInfo.getFile();

  // check, if file has valid path, already exists, etc..
  if( targetFile.existsAsFile() )
  {
    if( showOverwriteAudioFileWarningBox(targetFile.getFileName()) == false )
    {
      // user has canceled the action - unpress the record button and return:
      recordButton->setToggleState(false, false);
      return;
    }
  }

  recorder.setRecordingOptions(targetFile, 
    deviceManager->getCurrentAudioDevice()->getCurrentSampleRate(), 
    fileInfo.numChannels, fileInfo.bitDepth);

  recorder.startRecording();
}

void MixsonicAudioRecordDialog::stopRecording()
{
  // stop and update the target file to write into a new file when the user records again:
  recorder.stopRecording(true);
  textEntryField->setText(
    fileInfo.getFile().getNonexistentSibling().getFileNameWithoutExtension(), false);
  updateAudioFileInfoAccordingToWidgets();
}
