#include "MComboBox.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction and static member initialization:

int    MComboBox::instanceCounter  = 0;
Colour MComboBox::backgroundColour = Colour(0xffffffff);
Colour MComboBox::outlineColour    = Colour(0xff000000);
Colour MComboBox::textColour       = Colour(0xff000000);
Colour MComboBox::buttonColour     = Colour(0xff7777ff);
Colour MComboBox::arrowColour      = Colour(0xff000000);

MComboBox::MComboBox(Skin *skinToUse, const String& componentName) 
: ComboBox(componentName)
, MWidget(skinToUse)
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

MComboBox::~MComboBox()
{
  instanceCounter--;
}

//-------------------------------------------------------------------------------------------------
// others:

void MComboBox::loadColorScheme()
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

void MComboBox::mouseEnter(const juce::MouseEvent &e)
{
  MWidget::mouseEnter(e);
}

void MComboBox::mouseExit(const MouseEvent &e)
{
  MWidget::mouseExit(e);
}