#include "MixsonicModalDialog.h"

MixsonicModalDialog::MixsonicModalDialog()
{
  addAndMakeVisible( headlineLabel = new Label(String("Headline"), String("Headline")) );
  headlineLabel->setFont(Font(18, Font::bold));
  headlineLabel->setJustificationType(Justification::centred);
}

MixsonicModalDialog::~MixsonicModalDialog()
{
  deleteAllChildren();
}

//-------------------------------------------------------------------------------------------------
// setup

void MixsonicModalDialog::setHeadline(const String& newHeadline)
{
  headlineLabel->setText(newHeadline, false);
}

//-------------------------------------------------------------------------------------------------
// callbacks:

void MixsonicModalDialog::paint(Graphics &g)
{
  //g.fillAll(Colours::white);
  //g.setColour(mixsonicOrange);
  g.fillAll(backgroundColor);
  g.setColour(outlineColor);
  g.drawRect(0, 0, getWidth(), getHeight(), 3);
}

void MixsonicModalDialog::resized()
{
  headlineLabel->setBounds(0, 4, getWidth(), 16);
}


