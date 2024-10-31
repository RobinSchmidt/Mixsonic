#include "MLabel.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction:

MLabel::MLabel(Skin *skinToUse, const String& componentName, const String& labelText) 
: Label(componentName, labelText)
, MWidget(skinToUse, labelText) // \todo maybe replace labelText with a description here
{
  setColour(Label::outlineColourId,    skin->outlineColor);
  setColour(Label::backgroundColourId, skin->backgroundColor);
  setColour(Label::textColourId,       skin->foregroundColor);
  setFont(skin->normalFont);
}

MLabel::~MLabel()
{

}

//-------------------------------------------------------------------------------------------------
// others:

void MLabel::mouseEnter(const juce::MouseEvent &e)
{
  MWidget::mouseEnter(e);
}

void MLabel::mouseExit(const MouseEvent &e)
{
  MWidget::mouseExit(e);
}

//=================================================================================================
  
MHeadlineLabel::MHeadlineLabel(Skin *skinToUse, const String& componentName,  
  const String& labelText)
: MLabel(skinToUse, componentName, labelText)
{
  setColour(Label::backgroundColourId, skin->backgroundColor);
  setColour(Label::textColourId,       skin->foregroundColor);
  setFont(skin->bigFont);
}