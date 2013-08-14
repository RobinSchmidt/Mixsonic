#include "RLabel.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction:

RLabel::RLabel(const String& componentName, const String& labelText) 
: Label(componentName, labelText)
{
  //setColour(Label::backgroundColourId, Skin::getInstance()->backgroundColor);
  //setColour(Label::outlineColourId, Skin::getInstance()->outlineColor);
  //setColour(Label::textColourId, Skin::getInstance()->textColor);

  // invert color-scheme (make a subclass ColorInvertedLabel, or maybe headlineLabel later):
  setColour(Label::backgroundColourId, Skin::getInstance()->textColor);
  setColour(Label::outlineColourId, Skin::getInstance()->outlineColor);
  setColour(Label::textColourId, Skin::getInstance()->backgroundColor);

  setFont(Skin::getInstance()->widgetFont);
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