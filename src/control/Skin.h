#ifndef Skin_h
#define Skin_h

#include "../../libs/JuceIncludes.h"


/** A class for storing colors for various aspects of the graphical user interface. */

class ColorScheme
{

public:

  ColorScheme();

  virtual void initColors();

  // \todo getAsXml, setFromXml

  Colour widgetBackgroundColor, 
         backgroundColor, 
         highlightBackgroundColor, 
         outlineColor, 
         highlightOutlineColor, 
         textColor, 
         highlightTextColor;

  JUCE_LEAK_DETECTOR(ColorScheme);
};

//=================================================================================================

/** A class for storing info about the appearance of the graphical user interface. */

class Skin
{

public:

  //Typeface headlineTypeFace, widgetTypeface, infoTypeFace, etc... - or maybe use
  //class Font instead of Typeface

  // maybe factor out into a class widgetSizes:
  // int widgetOutlineThickness, smallButtonSize, standardButtonSize, bigButtonSize

  // getAsXml, setFromXml, loadFromXmlFile, saveAsXmlFile, etc....

  ColorScheme colorScheme;

  JUCE_LEAK_DETECTOR(Skin);
};



#endif
