#include "MixsonicTextEntryField.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction:


MixsonicTextEntryField::MixsonicTextEntryField(Skin *skinToUse, const String& initialText) 
: TextEditor()
, RWidget(skinToUse)
{
  setColour(Label::backgroundColourId, Colours::white);
  setColour(Label::outlineColourId, Colours::black);
  setText(initialText, false);
}

MixsonicTextEntryField::~MixsonicTextEntryField()
{

}

//-------------------------------------------------------------------------------------------------
// others:
/*
void MixsonicTextEntryField::mouseEnter(const juce::MouseEvent &e)
{
  RWidget::mouseEnter(e);
}

void MixsonicTextEntryField::mouseExit(const MouseEvent &e)
{
  RWidget::mouseExit(e);
}
*/