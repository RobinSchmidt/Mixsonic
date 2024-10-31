#include "MButton.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction and static member initialization:

int    MButton::instanceCounter  = 0;
Colour MButton::backgroundColour = Colour(0x00000000);
Colour MButton::outlineColour    = Colour(0xff000000);
Colour MButton::textColour       = Colour(0xff000000);
//Colour MButton::onStateColour    = Colour(0xffaaaacc);
//Colour MButton::offStateColour   = Colour(0xffccccff);
Colour MButton::onStateColour    = Colour(0xffaaaaaa);
Colour MButton::offStateColour   = Colour(0xffcccccc);
 // maybe, we should have an onStateTextColour (bright) and an offStateTextColour (dark)

MButton::MButton(Skin *skinToUse, int newSymbolIndex) 
: ToggleButton(String("SymbolButton"))
, MWidget(skinToUse)
{
  symbolIndex = newSymbolIndex;
  instanceCounter++;
  if( instanceCounter == 1 )
    loadColorScheme();
}

MButton::MButton(Skin *skinToUse, const String& componentName) 
: ToggleButton(componentName)
, MWidget(skinToUse)
{
  symbolIndex = 0;
  instanceCounter++;
  if( instanceCounter == 1 )
    loadColorScheme();
}

MButton::~MButton()
{
  instanceCounter--;
}

//-------------------------------------------------------------------------------------------------
// setup:

void MButton::setSymbolIndex(int newSymbolIndex)
{
  symbolIndex = newSymbolIndex;
}

//-------------------------------------------------------------------------------------------------
// others:

void MButton::loadColorScheme()
{
  String dir = getApplicationDirectoryAsString();
  File colorFile = File( dir + String("/ColorScheme.xml") );
  if( colorFile.existsAsFile() )
  {
    XmlDocument myDocument(colorFile);
    XmlElement* xmlColorScheme = myDocument.getDocumentElement();
    String colorString;
    if( xmlColorScheme != NULL )
    {
      XmlElement* xmlColors = xmlColorScheme->getChildByName("Buttons");
      if( xmlColors != NULL )
      {
        colorString = xmlColors->getStringAttribute("Background", "00000000" );
        backgroundColour = Colour::fromString(colorString);
        colorString = xmlColors->getStringAttribute("Outline", "ff000000" );
        outlineColour = Colour::fromString(colorString);
        colorString = xmlColors->getStringAttribute("Text", "ff000000" );
        textColour = Colour::fromString(colorString);
        colorString    = xmlColors->getStringAttribute("Off", "ffccccff" );
        offStateColour = Colour::fromString(colorString);
        colorString    = xmlColors->getStringAttribute("On", "ffaaaacc" );
        onStateColour = Colour::fromString(colorString);
      }
      delete xmlColorScheme;
    }
  }
}

void MButton::mouseEnter(const juce::MouseEvent &e)
{
  MWidget::mouseEnter(e);
  repaint();
}

void MButton::mouseExit(const MouseEvent &e)
{
  MWidget::mouseExit(e);
  repaint();
}

void MButton::paint(Graphics &g)
{
  paint2D(g);
  //paintPseudo3D(g);
}

void MButton::paint2D(Graphics &g) const
{
  float w = (float) getWidth();
  float h = (float) getHeight();

  if( getToggleState() || isDown() )
  {
    g.fillAll(skin->middlegroundColor);
    g.setColour(skin->outlineColorHighlight);
    g.drawRect(0.f, 0.f, w, h, 1.f);
    g.setColour(skin->foregroundColorHighlight);
  }
  else
  {
    g.fillAll(skin->backgroundColor);
    g.setColour(skin->outlineColor);
    g.drawRect(0.f, 0.f, w, h, 1.f);
    g.setColour(skin->foregroundColor);
  }

  if( isMouseOver() )
  {
    g.setColour(skin->outlineColorHighlight);
    g.drawRect(0.f, 0.f, w, h, 1.f);
  }

  if( symbolIndex <= 0 || symbolIndex > NUM_SYMBOLS )
  {
    g.setFont(skin->normalFont);
    g.drawText(getButtonText(), 0, 0, (int)w, (int)h, Justification::centred, false);
  }
  else
    drawSymbol(g);
}

void MButton::paintPseudo3D(Graphics &g) const
{
  int w = getWidth();
  int h = getHeight();

  // draw the pseudo 3D button with bevels:
  if( getToggleState() || isDown() )
  {
    g.fillAll(onStateColour);
    g.drawBevel(0, 0, w, h, 4, Colours::black, onStateColour.brighter(0.5), true);
  }
  else
  {
    g.fillAll(offStateColour);
    g.drawBevel(0, 0, w, h, 4, Colours::white, Colours::black, true);
    //g.drawBevel(0, 0, w+1, h+2, 4, Colours::white, Colours::black, true);
  }

  // draw the outline:
  g.setColour(outlineColour);  
  g.drawRect(0, 0, w, h, 1);

  // draw the text or symbol:
  g.setColour(textColour);
  if( symbolIndex <= 0 || symbolIndex > NUM_SYMBOLS )
    g.drawText(getButtonText(), 3, 2, w-6, h-6, Justification::centred, false);
  else
    drawSymbol(g);
}

void MButton::drawSymbol(Graphics &g) const
{
  // \todo get rid of the absolute values, use only relative values

  float x1, x2, y1, y2;
  switch( symbolIndex )
  {
  case PLUS:
    {
      x1 = 0.5f*getWidth();
      x2 = x1;
      y1 = getHeight()-4.f;
      y2 = 4.f;
      g.drawLine(x1, y1, x2, y2, 2.f);

      x1 = 4.f;
      x2 = getWidth()-4.f;
      y1 = 0.5f*(getHeight());
      y2 = y1;
      g.drawLine(x1, y1, x2, y2, 2.f);
    }
    break;

  case MINUS:
    {
      x1 = 4.f;
      x2 = getWidth()-4.f;
      y1 = 0.5f*(getHeight());
      y2 = y1;
      g.drawLine(x1, y1, x2, y2, 2.f);
    }
    break;

  case PLAY:
    {
      Path path;
      path.addTriangle(12.f, 8.f, getWidth()-12.f, 0.5f*getHeight(), 12.f, getHeight()-8.f);
      g.fillPath(path);
    }
    break;

  case SKIP_FORWARD:
    {
      Path path;
      path.addTriangle(8.f, 4.f, getWidth()-8.f, 0.5f*getHeight(), 8.f, getHeight()-4.f);
      path.addLineSegment(Line<float>(getWidth()-8.f, 4.f, getWidth()-8.f, getHeight()-4.f), 2.f);
      g.fillPath(path);
    }
    break;

  case SKIP_BACK:
    {
      Path path;
      path.addTriangle(getWidth()-8.f, 4.f, 8.f, 0.5f*getHeight(), getWidth()-8.f, getHeight()-4.f);
      path.addLineSegment(Line<float>(8.f, 4.f, 8.f, getHeight()-4.f), 2.f);
      g.fillPath(path);
    }
    break;


  }

}