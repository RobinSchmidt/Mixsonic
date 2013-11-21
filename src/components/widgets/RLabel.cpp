#include "RLabel.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction:

RLabel::RLabel(Skin *skinToUse, const String& componentName, const String& labelText) 
: Label(componentName, labelText)
, RWidget(skinToUse, labelText) // \todo maybe replace labelText with a description here
{
  setColour(Label::outlineColourId,    skin->outlineColor);
  setColour(Label::backgroundColourId, skin->backgroundColor);
  setColour(Label::textColourId,       skin->foregroundColor);
  setFont(skin->normalFont);
}

RLabel::~RLabel()
{

}

//-------------------------------------------------------------------------------------------------
// others:

void RLabel::mouseEnter(const juce::MouseEvent &e)
{
  RWidget::mouseEnter(e);
}

void RLabel::mouseExit(const MouseEvent &e)
{
  RWidget::mouseExit(e);
}

//=================================================================================================
  
RHeadlineLabel::RHeadlineLabel(Skin *skinToUse, const String& componentName,  
  const String& labelText)
: RLabel(skinToUse, componentName, labelText)
{
  setColour(Label::backgroundColourId, skin->backgroundColor);
  setColour(Label::textColourId,       skin->foregroundColor);
  setFont(skin->bigFont);
  /*
  setColour(Label::backgroundColourId, Skin::getInstance()->headlineBackgroundColor);
  setColour(Label::textColourId,       Skin::getInstance()->headlineTextColor);
  setFont(Skin::getInstance()->headlineFont);
  */
}