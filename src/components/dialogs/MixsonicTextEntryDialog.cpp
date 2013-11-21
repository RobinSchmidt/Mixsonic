#include "MixsonicTextEntryDialog.h"

MixsonicTextEntryDialog::MixsonicTextEntryDialog(SectionSkin *skinToUse, const String& labelText,                                               
  const String& initialText)
: MixsonicModalDialog(skinToUse)
{
  textEntryLabel = new RLabel(&skin->labelSkin, labelText, labelText);
  addAndMakeVisible(textEntryLabel);
  textEntryLabel->setJustificationType(Justification::centredRight);

  textEntryField = new MixsonicTextEntryField(&skin->widgetSkin, initialText);
  addAndMakeVisible(textEntryField);
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
