#include "HalfTransparentButton.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction and static member initialization:

Colour HalfTransparentButton::onStateColour    = Colour(0xffff0000);
Colour HalfTransparentButton::offStateColour   = Colour(0x00ffffff);

HalfTransparentButton::HalfTransparentButton(const String& buttonName) : TextButton(buttonName)
{
  transparency = 0.75f;
}

HalfTransparentButton::~HalfTransparentButton()
{

}

//-------------------------------------------------------------------------------------------------
// setup:

void HalfTransparentButton::setTransparency(float newTransparency)
{
  transparency = newTransparency;
}

void HalfTransparentButton::paint(Graphics &g)
{
  //int x = 0;
  //int y = 0;
  int w = getWidth();
  int h = getHeight();

  // draw the button background as a transparent colour:
  if( getToggleState() || isDown() )
    g.fillAll(onStateColour.withAlpha(1.f-transparency));
  else
    g.fillAll(offStateColour.withAlpha(1.f-transparency));

  // draw the outline:
  g.setColour(Colours::black);  
  g.drawRect(0, 0, w, h, 1);

  // draw the text:
  g.setColour(Colours::black);
  g.drawText(getButtonText(), 4, 4, w-8, h-8, Justification::centred, false);
}

