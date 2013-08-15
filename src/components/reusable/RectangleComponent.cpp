#include "RectangleComponent.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction:

RectangleComponent::RectangleComponent(const Colour &newFillColor, const Colour &newOutlineColor)
: Component(String("RectangleComponent"))
{
  fillColor    = newFillColor;
  outlineColor = newOutlineColor;
}

//-------------------------------------------------------------------------------------------------
// setup:
 
void RectangleComponent::setFillColor(const Colour &newFillColor)
{
  fillColor = newFillColor;
  repaint();
}

void RectangleComponent::setOutlineColor(const Colour &newOutlineColor)
{
  outlineColor = newOutlineColor;
  repaint();
}

//-------------------------------------------------------------------------------------------------
// others:

void RectangleComponent::paint(Graphics &g)
{
  g.setColour(fillColor);
  g.fillRect(0, 0, getWidth(), getHeight());
  g.setColour(outlineColor);
  g.drawRect(0, 0, getWidth(), getHeight());
}
