#include "MixsonicAudioExportDialog.h"

MixsonicAudioExportDialog::MixsonicAudioExportDialog(Arrangement *arrangementToExport, 
                                                     const File& defaultTargetFile) 
: MixsonicAudioFileSettingsDialog(defaultTargetFile)
{
  setHeadline(renderHeadlineStr);
  theArrangement = arrangementToExport;

  // create the additional button:
  addAndMakeVisible( renderButton = new RButton(renderButtonStr) );
  renderButton->setDescription(renderButtonHelpStr);
  renderButton->setClickingTogglesState(false);
  renderButton->addListener(this);

  // initialize the target file:
  File targetFile = File(getApplicationDirectoryAsString() + String("/ExportedAudio.wav") );
  if( defaultTargetFile != File::nonexistent )
    targetFile = defaultTargetFile;
  textEntryField->setText(targetFile.getFileNameWithoutExtension() , false);
  updateAudioFileInfoAccordingToWidgets();

  numChannelsComboBox->setEnabled(false);

  setSize(700, 74);
}

MixsonicAudioExportDialog::~MixsonicAudioExportDialog()
{
  deleteAllChildren();
}

//-------------------------------------------------------------------------------------------------
// setup:

void MixsonicAudioExportDialog::setDescriptionField(Label* newDescriptionField)
{
  MixsonicAudioFileSettingsDialog::setDescriptionField(newDescriptionField);
  renderButton->setDescriptionField(newDescriptionField);
}

//-------------------------------------------------------------------------------------------------
// callbacks:

void MixsonicAudioExportDialog::buttonClicked(Button *buttonThatWasClicked)
{
  if( buttonThatWasClicked == renderButton )
  {
    renderNow();
    exitModalState(1);
  }
  else
    MixsonicAudioFileSettingsDialog::buttonClicked(buttonThatWasClicked);
}

void MixsonicAudioExportDialog::resized()
{
  MixsonicAudioFileSettingsDialog::resized();

  renderButton->setBounds(exitButton->getX()-92, exitButton->getY(), 88, 20);
  int x = textEntryLabel->getRight();
  int w = renderButton->getX()-x;
  textEntryField->setBounds(x+4, exitButton->getY(), w-10, 20);
}

//-------------------------------------------------------------------------------------------------
// internal functions:

void MixsonicAudioExportDialog::renderNow()
{
  File targetFile = fileInfo.getFile();

  // check, if file has valid path, already exists, etc..
  if( targetFile.existsAsFile() )
  {
    if( showOverwriteAudioFileWarningBox(targetFile.getFileName()) == false )
    {
      // user has canceled the action - return immediately:
      return;
    }
  }

  // targetFile O.K. - set up a rendering thread and begin rendering:
  AudioRenderingThread renderThread;
  renderThread.setArrangementToRender(theArrangement);
  renderThread.setRenderingSampleRate(fileInfo.sampleRate);
  if( renderThread.runThread(0) )  // 0 indicates lowest possible thread priority
  {
    // rendering thread finished normally - retrieve the result:
    AudioSampleBuffer* renderingResult = renderThread.getRenderingResult();

    // save the result to a file:
    saveAudioSampleBufferToFile(renderingResult, targetFile, fileInfo.sampleRate, 
      fileInfo.bitDepth, false);

    // show a message box that rendering and saving was successful:
    showAudioFileRenderedBox(targetFile.getFullPathName());

    // clear the temporarily allocated memory:
    delete renderingResult;
  }
  else
  {
    // user pressed the cancel button - show an appropriate message box:
    showRenderingAbortedBox();
  }
}
