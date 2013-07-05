#include "Skin.h"

  
ColorScheme::ColorScheme()
{
  initColors();
}
 
void ColorScheme::initColors()
{
  widgetBackgroundColor    = Colour::Colour((uint8)  0, (uint8)  0, (uint8)  0, (uint8)255);
  backgroundColor          = Colour::Colour((uint8) 64, (uint8) 64, (uint8) 64, (uint8)255);
  highlightBackgroundColor = Colour::Colour((uint8) 96, (uint8) 96, (uint8) 96, (uint8)255);
  outlineColor             = Colour::Colour((uint8)192, (uint8)192, (uint8)192, (uint8)255);
  highlightOutlineColor    = Colour::Colour((uint8)255, (uint8)255, (uint8)255, (uint8)255);
  textColor                = Colour::Colour((uint8)192, (uint8)192, (uint8)192, (uint8)255);
  highlightTextColor       = Colour::Colour((uint8)255, (uint8)255, (uint8)255, (uint8)255);
}
