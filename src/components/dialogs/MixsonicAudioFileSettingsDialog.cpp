#include "MixsonicAudioFileSettingsDialog.h"

MixsonicAudioFileSettingsDialog::MixsonicAudioFileSettingsDialog(const File& defaultTargetFile)
: MixsonicTextEntryDialog(targetFileNameStr)
{
  String fileName      = defaultTargetFile.getFileNameWithoutExtension();
  String directoryName = defaultTargetFile.getParentDirectory().getFullPathName();

  setDescription(targetFileNameHelpStr);

  // the widgets:
  addAndMakeVisible( targetDirectoryLabel = new RLabel(targetDirectoryStr, targetDirectoryStr) );
  targetDirectoryLabel->setDescription(targetDirectoryHelpStr);
  targetDirectoryLabel->setJustificationType(Justification::centredRight);
  targetDirectoryLabel->addListener(this);

  addAndMakeVisible(  targetDirectoryField = new RLabel(directoryName, directoryName) );
  targetDirectoryField->setDescription(targetDirectoryHelpStr);
  targetDirectoryField->setColour(Label::backgroundColourId, Colours::white);
  targetDirectoryField->setColour(Label::outlineColourId, Colours::black);
  targetDirectoryField->setEditable(false);

  addAndMakeVisible(formatLabel = new RLabel(formatStr1, formatStr1) );
  formatLabel->setDescription(formatStr2);
  formatLabel->setJustificationType(Justification::centredRight);

  addAndMakeVisible( formatComboBox = new RComboBox(String("formatComboBox")) );
  formatComboBox->setDescription(formatStr2);
  formatComboBox->addItem(String("wav"),   1);
  formatComboBox->addItem(String("flac"),  2);
  formatComboBox->addItem(String("aiff"),  3);
  formatComboBox->setSelectedId(1, true);
  formatComboBox->addListener(this);

  addAndMakeVisible(numChannelsLabel = new RLabel(numChannelsStr1, numChannelsStr1) );
  numChannelsLabel->setDescription(numChannelsStr2);
  numChannelsLabel->setJustificationType(Justification::centredRight);

  addAndMakeVisible( numChannelsComboBox = new RComboBox(String("numChannelsComboBox")) );
  numChannelsComboBox->setDescription(numChannelsStr2);
  numChannelsComboBox->addItem(monoStr,   1);
  numChannelsComboBox->addItem(stereoStr, 2);
  numChannelsComboBox->setSelectedId(2, true);
  numChannelsComboBox->addListener(this);

  addAndMakeVisible(sampleRateLabel = new RLabel(sampleRateStr1, sampleRateStr1) );
  sampleRateLabel->setDescription(sampleRateStr2);
  sampleRateLabel->setJustificationType(Justification::centredRight);

  addAndMakeVisible( sampleRateComboBox = new RComboBox(String("sampleRateComboBox")) );
  sampleRateComboBox->setDescription(sampleRateStr2);
  sampleRateComboBox->addItem(String("44.1 kHz"),   1);
  sampleRateComboBox->addItem(String("48 kHz"),     2);
  sampleRateComboBox->addItem(String("88.2 kHz"),   3);
  sampleRateComboBox->addItem(String("96 kHz"),     4);
  sampleRateComboBox->setSelectedId(1, true);
  sampleRateComboBox->addListener(this);

  addAndMakeVisible(bitDepthLabel = new RLabel(bitDepthStr1, bitDepthStr1) );
  bitDepthLabel->setDescription(bitDepthStr2);
  bitDepthLabel->setJustificationType(Justification::centredRight);

  addAndMakeVisible( bitDepthComboBox = new RComboBox(String("bitDepthComboBox")) );
  bitDepthComboBox->setDescription(bitDepthStr2);
  bitDepthComboBox->addItem(String("16"),       1);
  bitDepthComboBox->addItem(String("24"),       2);
  //bitDepthComboBox->addItem(String("32 float"), 3);
  bitDepthComboBox->setSelectedId(1, true);
  bitDepthComboBox->addListener(this);

  // the buttons:
  addAndMakeVisible( browseButton = new RButton(browseButtonStr) );
  browseButton->setDescription(directoryBrowseButtonHelpStr);
  browseButton->setClickingTogglesState(false);
  browseButton->addListener(this);

  addAndMakeVisible( exitButton = new RButton(exitStr) );
  exitButton->setDescription(cancelStr);
  exitButton->setClickingTogglesState(false);
  exitButton->addListener(this);

  // initialize the target file field:
  File targetFile = File(getApplicationDirectoryAsString() + String("/AudioFile.wav") );
  if( defaultTargetFile != File::nonexistent )
    targetFile = defaultTargetFile;
  textEntryField->setText(targetFile.getFileNameWithoutExtension() , false);

  // initialize the passed AudioFileInfo object:
  updateAudioFileInfoAccordingToWidgets();

  setSize(640, 74);
}

MixsonicAudioFileSettingsDialog::~MixsonicAudioFileSettingsDialog()
{
  deleteAllChildren();
}

//-------------------------------------------------------------------------------------------------
// setup:

void MixsonicAudioFileSettingsDialog::setDescriptionField(Label* newDescriptionField)
{
  MixsonicTextEntryDialog::setDescriptionField(newDescriptionField);
  targetDirectoryLabel->setDescriptionField(newDescriptionField);
  targetDirectoryField->setDescriptionField(newDescriptionField);
  formatLabel->setDescriptionField(newDescriptionField);
  sampleRateLabel->setDescriptionField(newDescriptionField);
  bitDepthLabel->setDescriptionField(newDescriptionField);
  formatComboBox->setDescriptionField(newDescriptionField);
  sampleRateComboBox->setDescriptionField(newDescriptionField);
  bitDepthComboBox->setDescriptionField(newDescriptionField);
  browseButton->setDescriptionField(newDescriptionField);
  exitButton->setDescriptionField(newDescriptionField);
}

//-------------------------------------------------------------------------------------------------
// callbacks:

void MixsonicAudioFileSettingsDialog::buttonClicked(Button *buttonThatWasClicked)
{
  if( buttonThatWasClicked == browseButton )
  {
    FileChooser chooser(selectTargetDirectoryStr, File(targetDirectoryField->getText()), 
      String::empty, true);
    if( chooser.browseForDirectory() )
    {
      File newTargetDirectory = chooser.getResult();
      targetDirectoryField->setText(newTargetDirectory.getFullPathName(), false);
    }
  }
  else if( buttonThatWasClicked == exitButton )
    exitModalState(0);
}

void MixsonicAudioFileSettingsDialog::comboBoxChanged(juce::ComboBox *comboBoxThatHasChanged)
{
  updateAudioFileInfoAccordingToWidgets();
}

void MixsonicAudioFileSettingsDialog::labelTextChanged(Label *labelThatHasChanged)
{
  updateAudioFileInfoAccordingToWidgets();
}

void MixsonicAudioFileSettingsDialog::resized()
{
  MixsonicModalDialog::resized();

  int x = 0;
  int y = headlineLabel->getBottom();
  int w = getWidth();
  //int h = getHeight();

  textEntryLabel->setBounds(x+4,  y+4, 176, 20);
  exitButton->setBounds(         w-78, y+4, 72,  20);  
  x = textEntryLabel->getRight();
  w = exitButton->getX()-x;
  textEntryField->setBounds(x+4, y+4, w-8, 20);

  x = 0;
  w = getWidth();
  y = textEntryField->getBottom();

  targetDirectoryLabel->setBounds(x+4,  y+4, 176, 20);
  browseButton->setBounds(        w-78, y+4,  72, 20);  
  x = targetDirectoryLabel->getRight();
  w = browseButton->getX()-x;
  targetDirectoryField->setBounds(x+4, y+4, w-8, 20);

  /*
  y += 24;
  formatLabel->setBounds(x+4,                                   y+4, 80, 20);
  formatComboBox->setBounds(formatLabel->getRight(),            y+4, 52, 20);
  numChannelsLabel->setBounds(formatComboBox->getRight()+4,     y+4, 72, 20);
  numChannelsComboBox->setBounds(numChannelsLabel->getRight(),  y+4, 64, 20);
  sampleRateLabel->setBounds(numChannelsComboBox->getRight()+4, y+4, 88, 20);
  sampleRateComboBox->setBounds(sampleRateLabel->getRight(),    y+4, 80, 20);
  bitDepthLabel->setBounds(sampleRateComboBox->getRight()+4,    y+4, 72, 20);
  bitDepthComboBox->setBounds(bitDepthLabel->getRight(),        y+4, 48, 20);
  y -= 24;
  x  = formatComboBox->getX();
  w  = bitDepthComboBox->getRight()-x;
  browseButton->setBounds(targetFileField->getRight()+4,        y+4, 56, 20);  
  y += 24;
  */
}

//-------------------------------------------------------------------------------------------------
// internal functions:

void MixsonicAudioFileSettingsDialog::updateAudioFileInfoAccordingToWidgets()
{
  fileInfo.numChannels  = numChannelsComboBox->getSelectedId();
  fileInfo.bitDepth     = bitDepthComboBox->getText().getIntValue();
  fileInfo.sampleRate   = 1000.0 * sampleRateComboBox->getText().getDoubleValue();

  String targetFilePath = targetDirectoryField->getText() + File::separatorString 
    + textEntryField->getText();
  fileInfo.setFile( File(targetFilePath).withFileExtension(formatComboBox->getText()) );
  textEntryField->setText(fileInfo.getFile().getFileNameWithoutExtension(), false);
}

