#include "RComboBox.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction and static member initialization:

int    RComboBox::instanceCounter  = 0;
Colour RComboBox::backgroundColour = Colour(0xffffffff);
Colour RComboBox::outlineColour    = Colour(0xff000000);
Colour RComboBox::textColour       = Colour(0xff000000);
Colour RComboBox::buttonColour     = Colour(0xff7777ff);
Colour RComboBox::arrowColour      = Colour(0xff000000);

RComboBox::RComboBox(Skin *skinToUse, const String& componentName) 
: ComboBox(componentName)
, RWidget(skinToUse)
{
  instanceCounter++;
  if( instanceCounter == 1 )
    loadColorScheme();
  setColour(ComboBox::backgroundColourId, backgroundColour);
  setColour(ComboBox::outlineColourId,    outlineColour);
  setColour(ComboBox::textColourId,       textColour);
  setColour(ComboBox::buttonColourId,     buttonColour);
  setColour(ComboBox::arrowColourId,      arrowColour);
}

RComboBox::~RComboBox()
{
  instanceCounter--;
}

//-------------------------------------------------------------------------------------------------
// others:

void RComboBox::loadColorScheme()
{
  String dir = getApplicationDirectoryAsString();
  File colorFile = File( dir + String("/ColorScheme.xml") );
  if( colorFile.existsAsFile() )
  {
    XmlDocument myDocument(colorFile);
    XmlElement* xmlColorScheme = myDocument.getDocumentElement();
    String  colorString;
    if( xmlColorScheme != NULL )
    {
      XmlElement* xmlColors = xmlColorScheme->getChildByName("ComboBoxes");
      if( xmlColors != NULL )
      {
        colorString = xmlColors->getStringAttribute("Background", "ffffffff" );
        backgroundColour = Colour::fromString(colorString);
        colorString = xmlColors->getStringAttribute("Outline", "ff000000" );
        outlineColour = Colour::fromString(colorString);
        colorString = xmlColors->getStringAttribute("Text", "ff000000" );
        textColour = Colour::fromString(colorString);
        colorString = xmlColors->getStringAttribute("Button", "ff7777ff" );
        buttonColour = Colour::fromString(colorString);
        colorString = xmlColors->getStringAttribute("Arrow", "ff000000" );
        arrowColour = Colour::fromString(colorString);
      }
      delete xmlColorScheme;
    }
  }
}

void RComboBox::mouseEnter(const juce::MouseEvent &e)
{
  RWidget::mouseEnter(e);
}

void RComboBox::mouseExit(const MouseEvent &e)
{
  RWidget::mouseExit(e);
}