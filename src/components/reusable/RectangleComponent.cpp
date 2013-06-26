#include "RectangleComponent.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction:

RectangleComponent::RectangleComponent(const Colour &newFillColour, const Colour &newOutlineColour)
: Component(String("RectangleComponent"))
{
  fillColour    = newFillColour;
  outlineColour = newOutlineColour;
}

//-------------------------------------------------------------------------------------------------
// setup:

void RectangleComponent::setColour(const int colourId, const Colour &colour)
{
  if( colourId == fillColourId )
    fillColour = colour;
  else if( colourId == outlineColourId )
    outlineColour = colour;
  repaint();
}

//-------------------------------------------------------------------------------------------------
// others:

void RectangleComponent::paint(Graphics &g)
{
  g.setColour(fillColour);
  g.fillRect(0, 0, getWidth(), getHeight());
  g.setColour(outlineColour);
  g.drawRect(0, 0, getWidth(), getHeight());
}
