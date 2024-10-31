#include "MixsonicGlobalSettingsDialog.h"

MixsonicGlobalSettingsDialog::MixsonicGlobalSettingsDialog(SectionSkin *skinToUse, 
  MixsonicSettings* settingsToManipulate) 
: MixsonicModalDialog(skinToUse)
{
  setHeadline(globalSettingsHeadlineStr);
  settings    = settingsToManipulate;
  settingsTmp = *settings;

  projectsParentDirectoryLabel = new MLabel(&skin->labelSkin, projectsParentDirStr, 
    projectsParentDirStr);
  addAndMakeVisible(projectsParentDirectoryLabel);
  projectsParentDirectoryLabel->setDescription(projectsParentDirHelpStr);
  projectsParentDirectoryLabel->setJustificationType(Justification::centredLeft);

  String directoryName = settings->getProjectsParentDirectory().getFullPathName();
  projectsParentDirectoryTextField = new MixsonicTextEntryField(&skin->widgetSkin, directoryName);
  addAndMakeVisible(projectsParentDirectoryTextField);
  projectsParentDirectoryTextField->setDescription(projectsParentDirHelpStr);
  projectsParentDirectoryTextField->setColour(Label::backgroundColourId, Colours::white);
  projectsParentDirectoryTextField->setColour(Label::outlineColourId, Colours::black);
  projectsParentDirectoryTextField->setReadOnly(true);

  projectsParentDirectoryBrowseButton = new MButton(&skin->widgetSkin, browseButtonStr);
  addAndMakeVisible(projectsParentDirectoryBrowseButton);
  projectsParentDirectoryBrowseButton->setDescription(browseButtonStr);
  projectsParentDirectoryBrowseButton->setClickingTogglesState(false);
  projectsParentDirectoryBrowseButton->addListener(this);

  sampleContentDirectoryLabel = new MLabel(&skin->labelSkin, sampleContentDirStr, 
    sampleContentDirStr);
  addAndMakeVisible(sampleContentDirectoryLabel);
  sampleContentDirectoryLabel->setDescription(sampleContentDirHelpStr);
  sampleContentDirectoryLabel->setJustificationType(Justification::centredLeft);

  directoryName = settings->getSampleContentDirectory().getFullPathName();
  sampleContentDirectoryTextField = new MixsonicTextEntryField(&skin->widgetSkin, directoryName);
  addAndMakeVisible(sampleContentDirectoryTextField);
  sampleContentDirectoryTextField->setDescription(sampleContentDirHelpStr);
  sampleContentDirectoryTextField->setColour(Label::backgroundColourId, Colours::white);
  sampleContentDirectoryTextField->setColour(Label::outlineColourId, Colours::black);
  sampleContentDirectoryTextField->setReadOnly(true);

  sampleContentDirectoryBrowseButton = new MButton(&skin->widgetSkin, browseButtonStr);
  addAndMakeVisible(sampleContentDirectoryBrowseButton);
  sampleContentDirectoryBrowseButton->setDescription(browseButtonStr);
  sampleContentDirectoryBrowseButton->setClickingTogglesState(false);
  sampleContentDirectoryBrowseButton->addListener(this);



  //pluginDirectoriesList = new FileSearchPathListComponent;
  //pluginDirectoriesList->setPath(settings->getPluginDirectories());
  //addAndMakeVisible(pluginDirectoriesList);


  cancelButton = new MButton(&skin->widgetSkin, cancelStr);
  addAndMakeVisible(cancelButton);
  cancelButton->setDescription(cancelStr);
  cancelButton->setClickingTogglesState(false);
  cancelButton->addListener(this);

  okButton = new MButton(&skin->widgetSkin, okStr);
  addAndMakeVisible(okButton);
  okButton->setDescription(okStr);
  okButton->setClickingTogglesState(false);
  okButton->addListener(this);

  setSize(700, 98);
}

MixsonicGlobalSettingsDialog::~MixsonicGlobalSettingsDialog()
{
  deleteAllChildren();
}

//-------------------------------------------------------------------------------------------------
// setup:

void MixsonicGlobalSettingsDialog::setDescriptionField(Label* newDescriptionField)
{
  projectsParentDirectoryLabel->setDescriptionField(newDescriptionField);
  projectsParentDirectoryTextField->setDescriptionField(newDescriptionField);
  projectsParentDirectoryBrowseButton->setDescriptionField(newDescriptionField);
  sampleContentDirectoryLabel->setDescriptionField(newDescriptionField);
  sampleContentDirectoryTextField->setDescriptionField(newDescriptionField);
  sampleContentDirectoryBrowseButton->setDescriptionField(newDescriptionField);
  cancelButton->setDescriptionField(newDescriptionField);
  okButton->setDescriptionField(newDescriptionField);
}

//-------------------------------------------------------------------------------------------------
// callbacks:

void MixsonicGlobalSettingsDialog::buttonClicked(Button *buttonThatWasClicked)
{
  if( buttonThatWasClicked == cancelButton )
    exitModalState(0);  // 0 as return value indicates that no changes have been made
  else if( buttonThatWasClicked == okButton )
  {
    *settings = settingsTmp;
    settings->saveToFile();
    exitModalState(1);  // 1 as return value indicates that (possibly) changes have been made
  }
  else if( buttonThatWasClicked == projectsParentDirectoryBrowseButton ) 
  {
    FileChooser chooser(selectProjectsParentDirStr, 
      File(projectsParentDirectoryTextField->getText()), String::empty, true);
    if( chooser.browseForDirectory() )
    {
      File newDirectory = chooser.getResult();
      settingsTmp.setProjectsParentDirectory(newDirectory);
      projectsParentDirectoryTextField->setText(newDirectory.getFullPathName(), false);
    }
  }
  else if( buttonThatWasClicked == sampleContentDirectoryBrowseButton ) 
  {
    FileChooser chooser(selectSampleContentDirStr, 
      File(sampleContentDirectoryTextField->getText()), String::empty, true);
    if( chooser.browseForDirectory() )
    {
      File newDirectory = chooser.getResult();
      settingsTmp.setSampleContentDirectory(newDirectory);
      sampleContentDirectoryTextField->setText(newDirectory.getFullPathName(), false);
    }
  }
}

void MixsonicGlobalSettingsDialog::resized()
{
  MixsonicModalDialog::resized();

  int x = 0;
  int y = headlineLabel->getBottom();
  int w = getWidth();
  //int h = getHeight();

  projectsParentDirectoryLabel->setBounds(       x+4,  y+4, 60, 20);
  projectsParentDirectoryBrowseButton->setBounds(w-78, y+4, 72, 20);  
  x = projectsParentDirectoryLabel->getRight();
  w = projectsParentDirectoryBrowseButton->getX()-x;
  projectsParentDirectoryTextField->setBounds(x+4, y+4, w-8, 20);

  x = 0;
  w = getWidth();
  y = projectsParentDirectoryLabel->getBottom();
  sampleContentDirectoryLabel->setBounds(       x+4,  y+4, 60, 20);
  sampleContentDirectoryBrowseButton->setBounds(w-78, y+4, 72, 20);  
  x = sampleContentDirectoryLabel->getRight();
  w = sampleContentDirectoryBrowseButton->getX()-x;
  sampleContentDirectoryTextField->setBounds(x+4, y+4, w-8, 20);

  x = sampleContentDirectoryBrowseButton->getX();
  y = sampleContentDirectoryBrowseButton->getBottom();
  w = sampleContentDirectoryBrowseButton->getWidth();
  cancelButton->setBounds(x, y+4, w, 20);

  x -= w+4;
  okButton->setBounds(x, y+4, w, 20);
}
