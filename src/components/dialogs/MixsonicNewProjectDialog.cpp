#include "MixsonicNewProjectDialog.h"

MixsonicNewProjectDialog::MixsonicNewProjectDialog(const File& projectParentDirectory)
: MixsonicTextEntryDialog(enterProjectNameStr)
{
  setHeadline(enterProjectNameHeadlineStr);

  parentDirectory        = projectParentDirectory;
  chosenProjectDirectory = File::nonexistent;

  setDescription(enterProjectNameHelpStr);

  addAndMakeVisible( createButton = new RButton(createStr) );
  createButton->setDescription(createProjectHelpStr);
  createButton->setClickingTogglesState(false);
  createButton->addListener(this);

  addAndMakeVisible( cancelButton = new RButton(cancelStr) );
  cancelButton->setDescription(cancelStr);
  cancelButton->setClickingTogglesState(false);
  cancelButton->addListener(this);

  setSize(400, 74);
}

MixsonicNewProjectDialog::~MixsonicNewProjectDialog()
{
  deleteAllChildren();
}

//-------------------------------------------------------------------------------------------------
// setup:

void MixsonicNewProjectDialog::setDescriptionField(Label* newDescriptionField)
{
  MixsonicTextEntryDialog::setDescriptionField(newDescriptionField);
  createButton->setDescriptionField(newDescriptionField);
  cancelButton->setDescriptionField(newDescriptionField);
}

//-------------------------------------------------------------------------------------------------
// inquiry

const String MixsonicNewProjectDialog::getChosenNewProjectName()
{
  return chosenProjectDirectory.getFileName();
}

//-------------------------------------------------------------------------------------------------
// callbacks:

void MixsonicNewProjectDialog::buttonClicked(Button *buttonThatWasClicked)
{
  if( buttonThatWasClicked == createButton )
  {
    if( textEntryField->getText() == String::empty )
    {
      // open a box to indicate that the user must enter something:
      showEnterNameErrorBox();
      return;
    }

    String path = parentDirectory.getFullPathName() + File::separatorString 
                  + textEntryField->getText();

    if( !isStringLegalPathName(path) )
    {
      showInvalidPathNameErrorBox(path);
      return;
    }

    chosenProjectDirectory = File(path);

    // we don't allow to create a new project directory, if such a directory already exists:
    if( chosenProjectDirectory.exists() )
      showProjectAlreadyExistsErrorBox(chosenProjectDirectory.getFileName());
    else
      exitModalState(1);
  }
  else if( buttonThatWasClicked == cancelButton )
    exitModalState(0);
}

void MixsonicNewProjectDialog::resized()
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
  createButton->setBounds(getWidth()-64-6, y+4, 64, 20);  

  x = createButton->getX();
  cancelButton->setBounds(x-64-6, y+4, 64, 20); 
}


