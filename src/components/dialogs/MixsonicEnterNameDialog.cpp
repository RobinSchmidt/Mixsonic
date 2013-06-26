#include "MixsonicEnterNameDialog.h"

MixsonicEnterNameDialog::MixsonicEnterNameDialog(const String& defaultName)
: MixsonicTextEntryDialog(enterNameStr)
{
  setHeadline(enterNameHeadlineStr);

  setDescription(enterNameStr);

  addAndMakeVisible( okButton = new RButton(okStr) );
  okButton->setDescription(createProjectHelpStr);
  okButton->setClickingTogglesState(false);
  okButton->addListener(this);

  addAndMakeVisible( cancelButton = new RButton(cancelStr) );
  cancelButton->setDescription(cancelStr);
  cancelButton->setClickingTogglesState(false);
  cancelButton->addListener(this);

  setSize(400, 74);
}

MixsonicEnterNameDialog::~MixsonicEnterNameDialog()
{
  deleteAllChildren();
}

//-------------------------------------------------------------------------------------------------
// setup:

void MixsonicEnterNameDialog::setDescriptionField(Label* newDescriptionField)
{
  MixsonicTextEntryDialog::setDescriptionField(newDescriptionField);
  okButton->setDescriptionField(newDescriptionField);
  cancelButton->setDescriptionField(newDescriptionField);
}

//-------------------------------------------------------------------------------------------------
// inquiry:

const String MixsonicEnterNameDialog::getChosenName()
{
  return textEntryField->getText();
}

//-------------------------------------------------------------------------------------------------
// callbacks:

void MixsonicEnterNameDialog::buttonClicked(Button *buttonThatWasClicked)
{
  if( buttonThatWasClicked == okButton )
    exitModalState(1);
  else if( buttonThatWasClicked == cancelButton )
    exitModalState(0);
}

void MixsonicEnterNameDialog::resized()
{
  MixsonicModalDialog::resized();

  int x = 0;
  int y = headlineLabel->getBottom();
  int w = getWidth();
  //int h = getHeight();

  textEntryLabel->setBounds(x+4, y+4, 160, 20);
  x = textEntryLabel->getRight();
  w = getWidth()-x;
  textEntryField->setBounds(x+4, y+4, w-10, 20);

  y = textEntryField->getBottom();
  okButton->setBounds(getWidth()-64-6, y+4, 64, 20);  

  x = okButton->getX();
  cancelButton->setBounds(x-64-6, y+4, 64, 20); 
}




