#include "Skin.h"

Skin* Skin::instance = nullptr;

Skin::Skin() 
{
  initialize();
  // \todo load skin from xml-file
}   

Skin* Skin::getInstance()
{
  if( instance == nullptr )
    instance = new Skin();
  return instance;
}

void Skin::releaseInstance()
{
  delete instance;
  instance = nullptr;
}

void Skin::initialize()
{
  backgroundColor          = getGrayValue( 64);
  highlightBackgroundColor = getGrayValue( 96);
  widgetBackgroundColor    = getGrayValue(  0); 
  widgetHandleColor        = getGrayValue( 96);
  outlineColor             = getGrayValue(192);
  highlightOutlineColor    = getGrayValue(255);
  textColor                = getGrayValue(192);
  highlightTextColor       = getGrayValue(255);
}
