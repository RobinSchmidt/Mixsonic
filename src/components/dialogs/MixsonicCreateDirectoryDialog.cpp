#include "MixsonicCreateDirectoryDialog.h"

MixsonicCreateDirectoryDialog::MixsonicCreateDirectoryDialog(const File& newParentDirectory)
: MixsonicTextEntryDialog(enterDirectoryNameStr)
{
  setHeadline(createDirectoryHeadlineStr);

  parentDirectory   = newParentDirectory;
  directoryToCreate = File::nonexistent;

  addAndMakeVisible( createButton = new RButton(createStr) );
  createButton->setDescription(createDirectoryHelpStr);
  createButton->setClickingTogglesState(false);
  createButton->addListener(this);

  addAndMakeVisible( cancelButton = new RButton(cancelStr) );
  cancelButton->setDescription(cancelStr);
  cancelButton->setClickingTogglesState(false);
  cancelButton->addListener(this);

  setSize(400, 74);
}

MixsonicCreateDirectoryDialog::~MixsonicCreateDirectoryDialog()
{
  deleteAllChildren();
}

//-------------------------------------------------------------------------------------------------
// setup:

void MixsonicCreateDirectoryDialog::setDescriptionField(Label* newDescriptionField)
{
  MixsonicTextEntryDialog::setDescriptionField(newDescriptionField);
  createButton->setDescriptionField(newDescriptionField);
  cancelButton->setDescriptionField(newDescriptionField);
}

//-------------------------------------------------------------------------------------------------
// inquiry

const File MixsonicCreateDirectoryDialog::getDirectoryToCreate()
{
  return directoryToCreate;
}

//-------------------------------------------------------------------------------------------------
// callbacks:

void MixsonicCreateDirectoryDialog::buttonClicked(Button *buttonThatWasClicked)
{
  if( buttonThatWasClicked == createButton )
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

    directoryToCreate = File(path);

    // we don't allow to create a new project directory, if such a directory already exists:
    if( directoryToCreate.exists() )
      showProjectAlreadyExistsErrorBox(directoryToCreate.getFileName());
    else
      exitModalState(1);
  }
  else if( buttonThatWasClicked == cancelButton )
    exitModalState(0);
}

void MixsonicCreateDirectoryDialog::resized()
{
  MixsonicModalDialog::resized();

  int x = 0;
  int y = headlineLabel->getBottom();
  int w = getWidth();
  //int h = getHeight();

  textEntryLabel->setBounds(x+4, y+4, 196, 20);
  x = textEntryLabel->getRight();
  w = getWidth()-x;
  textEntryField->setBounds(x+4, y+4, w-10, 20);

  y = textEntryField->getBottom();
  createButton->setBounds(getWidth()-64-6, y+4, 64, 20);  

  x = createButton->getX();
  cancelButton->setBounds(x-64-6, y+4, 64, 20); 
}




