#include "MixsonicButton.h"

MixsonicButton::MixsonicButton(Skin *skinToUse) 
: RButton(skinToUse, NO_SYMBOL)
, normalImage(0)
, pressedImage(0)
, hoverImage(0)
{

}

MixsonicButton::~MixsonicButton()
{

}

void MixsonicButton::setImages(const Image *newNormalImage, const Image *newPressedImage)
{
  normalImage  = newNormalImage;
  pressedImage = newPressedImage;
}

//-------------------------------------------------------------------------------------------------
// inquiry:

bool MixsonicButton::isPressed()
{
  if( (getClickingTogglesState() == true) && (getToggleState() == true) )
    return true;
  else if( (getClickingTogglesState() == false) && (isDown() == true) )
    return true;
  else
    return false;
}

//-------------------------------------------------------------------------------------------------
// others:

void MixsonicButton::paintButton(Graphics &g, bool isMouseOverButton, bool isButtonDown)
{
  if( normalImage == NULL || pressedImage == NULL )
  {
    //jassertfalse; // you must assign images for the two button states
    g.fillAll(Colours::red);
    return;
  }

  if( isPressed() == true )
  {
    g.setColour(Colours::black);
    g.drawImage(*pressedImage,
      0, 0, pressedImage->getWidth(), pressedImage->getHeight(),
      0, 0, pressedImage->getWidth(), pressedImage->getHeight());
  }
  else
  {
    g.setColour(Colours::black);
    g.drawImage(*normalImage,
      0, 0, normalImage->getWidth(), normalImage->getHeight(),
      0, 0, normalImage->getWidth(), normalImage->getHeight());
  }
}

/*
void MixsonicButton::mouseEnter(const MouseEvent &e)
{
  RWidget::mouseEnter(e);
}

void MixsonicButton::mouseExit(const MouseEvent &e)
{
  RWidget::mouseExit(e);
}
*/