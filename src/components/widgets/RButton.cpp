#include "RButton.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction and static member initialization:

int    RButton::instanceCounter  = 0;
Colour RButton::backgroundColour = Colour(0x00000000);
Colour RButton::outlineColour    = Colour(0xff000000);
Colour RButton::textColour       = Colour(0xff000000);
//Colour RButton::onStateColour    = Colour(0xffaaaacc);
//Colour RButton::offStateColour   = Colour(0xffccccff);
Colour RButton::onStateColour    = Colour(0xffaaaaaa);
Colour RButton::offStateColour   = Colour(0xffcccccc);
 // maybe, we should have an onStateTextColour (bright) and an offStateTextColour (dark)

RButton::RButton(int newSymbolIndex) : ToggleButton(String("SymbolButton"))
{
  symbolIndex = newSymbolIndex;
  instanceCounter++;
  if( instanceCounter == 1 )
    loadColorScheme();
}

RButton::RButton(const String& componentName) : ToggleButton(componentName)
{
  symbolIndex = 0;
  instanceCounter++;
  if( instanceCounter == 1 )
    loadColorScheme();
}

RButton::~RButton()
{
  instanceCounter--;
}

//-------------------------------------------------------------------------------------------------
// setup:

void RButton::setSymbolIndex(int newSymbolIndex)
{
  symbolIndex = newSymbolIndex;
}

//-------------------------------------------------------------------------------------------------
// others:

void RButton::loadColorScheme()
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

void RButton::mouseEnter(const juce::MouseEvent &e)
{
  RWidget::mouseEnter(e);
  repaint();
}

void RButton::mouseExit(const MouseEvent &e)
{
  RWidget::mouseExit(e);
  repaint();
}

void RButton::paint(Graphics &g)
{
  paint2D(g);
  //paintPseudo3D(g);
}

void RButton::paint2D(Graphics &g) const
{
  float w = (float) getWidth();
  float h = (float) getHeight();

  if( getToggleState() || isDown() )
  {
    //g.fillAll(highlightBackgroundColor);
    //g.fillAll(onStateColour);
    g.fillAll(Skin::getInstance()->widgetHandleColor);
    g.setColour(Skin::getInstance()->outlineHighlightColor);
    g.drawRect(0.f, 0.f, w, h, 1.f);
    g.setColour(Skin::getInstance()->textHighlightColor);
  }
  else
  {
    //g.fillAll(widgetBackgroundColor);
    g.fillAll(Skin::getInstance()->backgroundColor);
    g.setColour(Skin::getInstance()->outlineColor);
    g.drawRect(0.f, 0.f, w, h, 1.f);
    g.setColour(Skin::getInstance()->textColor);
  }

  if( isMouseOver() )
  {
    g.setColour(Skin::getInstance()->outlineHighlightColor);
    g.drawRect(0.f, 0.f, w, h, 1.f);
  }

  if( symbolIndex <= 0 || symbolIndex > NUM_SYMBOLS )
  {
    g.setFont(Skin::getInstance()->widgetFont);
    g.drawText(getButtonText(), 4, 4, (int)w-8, (int)h-8, Justification::centred, false);
    //g.drawText(getButtonText(), 3, 2, (int)w-6, (int)h-6, Justification::centred, false);
  }
  else
    drawSymbol(g);
}

void RButton::paintPseudo3D(Graphics &g) const
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

void RButton::drawSymbol(Graphics &g) const
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