#include "RLabel.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction:

RLabel::RLabel(const String& componentName, const String& labelText) 
: Label(componentName, labelText)
{
  setColour(Label::outlineColourId,    Skin::getInstance()->outlineColor);
  setColour(Label::backgroundColourId, Skin::getInstance()->backgroundColor);
  setColour(Label::textColourId,       Skin::getInstance()->textColor);
  setFont(Skin::getInstance()->labelFont);
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
  
RHeadlineLabel::RHeadlineLabel(const String& componentName,  const String& labelText)
: RLabel(componentName, labelText)
{
  setColour(Label::backgroundColourId, Skin::getInstance()->headlineBackgroundColor);
  setColour(Label::textColourId,       Skin::getInstance()->headlineTextColor);
  setFont(Skin::getInstance()->headlineFont);
}