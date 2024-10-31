#include "MixsonicEnterProjectNameDialog.h"

MixsonicEnterProjectNameDialog::MixsonicEnterProjectNameDialog(SectionSkin *skinToUse, 
  const File& projectParentDirectory)
: MixsonicTextEntryDialog(skinToUse, enterProjectNameStr)
{
  setHeadline(enterProjectNameHeadlineStr);

  parentDirectory = projectParentDirectory;
  //chosenProjectDirectory = File::nonexistent;

  setDescription(enterProjectNameHelpStr);

  okButton = new MButton(&skin->widgetSkin, okStr);
  addAndMakeVisible(okButton);
  okButton->setDescription(createProjectHelpStr);
  okButton->setClickingTogglesState(false);
  okButton->addListener(this);

  cancelButton = new MButton(&skin->widgetSkin, cancelStr);
  addAndMakeVisible(cancelButton);
  cancelButton->setDescription(cancelStr);
  cancelButton->setClickingTogglesState(false);
  cancelButton->addListener(this);

  setSize(400, 74);
}

MixsonicEnterProjectNameDialog::~MixsonicEnterProjectNameDialog()
{
  deleteAllChildren();
}

//-------------------------------------------------------------------------------------------------
// setup:

void MixsonicEnterProjectNameDialog::setDescriptionField(Label* newDescriptionField)
{
  MixsonicTextEntryDialog::setDescriptionField(newDescriptionField);
  okButton->setDescriptionField(newDescriptionField);
  cancelButton->setDescriptionField(newDescriptionField);
}

//-------------------------------------------------------------------------------------------------
// inquiry

const String MixsonicEnterProjectNameDialog::getChosenNewProjectName()
{
  return projectName;
}

//-------------------------------------------------------------------------------------------------
// callbacks:

void MixsonicEnterProjectNameDialog::buttonClicked(Button *buttonThatWasClicked)
{
  if( buttonThatWasClicked == okButton )
  {
    if( textEntryField->getText() == String::empty )
    {
      // open a box to indicate that the user must enter something:
      showEnterNameErrorBox();
      return;
    }

    String path =   parentDirectory.getFullPathName() + File::separatorString 
                  + textEntryField->getText();

    if( !isStringLegalPathName(path) )
    {
      showInvalidPathNameErrorBox(path);
      return;
    }

    File chosenProjectDirectory = File(path);

    // we don't allow to create a new project directory, if such a directory already exists:
    if( chosenProjectDirectory.exists() )
      showProjectAlreadyExistsErrorBox(chosenProjectDirectory.getFileName());
    else
    {
      projectName = chosenProjectDirectory.getFileName();
      exitModalState(1);
    }
  }
  else if( buttonThatWasClicked == cancelButton )
    exitModalState(0);
}

void MixsonicEnterProjectNameDialog::resized()
{
  MixsonicModalDialog::resized();

  int x = 0;
  int y = headlineLabel->getBottom();
  int w = getWidth();
  //int h = getHeight();

  textEntryLabel->setBounds(x+4, y+4, 164, 20);
  x = textEntryLabel->getRight();
  w = getWidth()-x;
  textEntryField->setBounds(x+4, y+4, w-10, 20);

  y = textEntryField->getBottom();
  okButton->setBounds(getWidth()-64-6, y+4, 64, 20);  

  x = okButton->getX();
  cancelButton->setBounds(x-64-6, y+4, 64, 20); 
}




