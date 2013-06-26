#include "MixsonicTextEntryDialog.h"

MixsonicTextEntryDialog::MixsonicTextEntryDialog(const String& labelText,
                                                 const String& initialText)
{
  addAndMakeVisible( textEntryLabel = new RLabel(labelText, labelText)        );
  textEntryLabel->setJustificationType(Justification::centredRight);

  addAndMakeVisible( textEntryField = new MixsonicTextEntryField(initialText) );
}

MixsonicTextEntryDialog::~MixsonicTextEntryDialog()
{
  deleteAllChildren();
}

//-------------------------------------------------------------------------------------------------
// setup:

void MixsonicTextEntryDialog::setText(const String &newText)
{
  textEntryField->setText(newText, false);
}

void MixsonicTextEntryDialog::setLabelText(const String &newText)
{
  textEntryLabel->setText(newText, false);
}

void MixsonicTextEntryDialog::setDescription(const juce::String &newDescription)
{
  textEntryField->setDescription(newDescription);
  textEntryLabel->setDescription(newDescription);
}

void MixsonicTextEntryDialog::setDescriptionField(Label *newDescriptionField)
{
  textEntryField->setDescriptionField(newDescriptionField);
  textEntryLabel->setDescriptionField(newDescriptionField);
}

//-------------------------------------------------------------------------------------------------
// callbacks:

void MixsonicTextEntryDialog::grabKeyboardFocus()
{
  MixsonicModalDialog::grabKeyboardFocus();
  textEntryField->grabKeyboardFocus();
}
