#include "RLabel.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction and static member initialization:

Colour RLabel::backgroundColour = textColor;
Colour RLabel::outlineColour    = textColor; 
Colour RLabel::textColour       = backgroundColor;

RLabel::RLabel(const String& componentName, const String& labelText) 
: Label(componentName, labelText)
{
  setColour(Label::backgroundColourId, backgroundColour);
  setColour(Label::outlineColourId, outlineColour);
  setColour(Label::textColourId, textColour);
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