#include "Skin.h"

Skin* Skin::instance = nullptr;

Skin::Skin() 
{
  initialize();
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

XmlElement* Skin::getAsXml()
{
  XmlElement *xmlSkin = new XmlElement("SKIN");

  // colors:
  XmlElement *xmlColors = new XmlElement("COLORS");
  xmlColors->setAttribute("background",          backgroundColor.toString());
  xmlColors->setAttribute("backgroundHighlight", backgroundHighlightColor.toString());
  xmlColors->setAttribute("widgetBackground",    widgetBackgroundColor.toString());
  xmlColors->setAttribute("widgetHandle",        widgetHandleColor.toString());
  xmlColors->setAttribute("outline",             outlineColor.toString());
  xmlColors->setAttribute("outlineHighlight",    outlineHighlightColor.toString());
  xmlColors->setAttribute("text",                textColor.toString());
  xmlColors->setAttribute("textHighlight",       textHighlightColor.toString());
  xmlSkin->addChildElement(xmlColors);

  // fonts:
  XmlElement *xmlFonts = new XmlElement("FONTS");
  addFontToXmlAndSerialise(*xmlFonts, textFont,     "textFont");
  addFontToXmlAndSerialise(*xmlFonts, widgetFont,   "widgetFont");
  addFontToXmlAndSerialise(*xmlFonts, headlineFont, "headlineFont");
  // maybe have an extra labelFont
  xmlSkin->addChildElement(xmlFonts);

  // \todo control further aspects, like the gradient colors for the level/pan sliders, outline 
  // thicknesses, images for buttons etc, widget sizes, etc.

  return xmlSkin;
}

void Skin::setFromXml(const XmlElement& xml)
{
  if( xml.getTagName() != "SKIN" )
  {
    jassertfalse;  // wrong kind of XML element
    return;
  }

  XmlElement *child = xml.getChildByName("COLORS");
  if( child != nullptr )
  {
    backgroundColor = Colour::fromString(child->getStringAttribute("background",    
      backgroundColor.toString()));
    backgroundHighlightColor = Colour::fromString(child->getStringAttribute("backgroundHighlight",    
      backgroundHighlightColor.toString()));
    widgetBackgroundColor = Colour::fromString(child->getStringAttribute("widgetBackground",    
      widgetBackgroundColor.toString()));
    widgetHandleColor = Colour::fromString(child->getStringAttribute("widgetHandle",    
      widgetHandleColor.toString()));
    outlineColor = Colour::fromString(child->getStringAttribute("outline",    
      outlineColor.toString()));
    outlineHighlightColor = Colour::fromString(child->getStringAttribute("outlineHighlight",    
      outlineHighlightColor.toString()));
    textColor = Colour::fromString(child->getStringAttribute("text",    
      textColor.toString()));
    textHighlightColor = Colour::fromString(child->getStringAttribute("textHighlight",    
      textHighlightColor.toString()));
  }

  child = xml.getChildByName("FONTS");
  if( child != nullptr )
  {
    retrieveFontFromXml(*child, textFont,     String("textFont"));
    retrieveFontFromXml(*child, widgetFont,   String("widgetFont"));
    retrieveFontFromXml(*child, headlineFont, String("headlineFont"));
  }

  // \todo retrieve sizes, images, etc.
}

void Skin::loadFromXmlFile(const File& file)
{
  XmlElement* xmlSkin = loadXmlFromFile(file);
  if( xmlSkin != nullptr )
    setFromXml(*xmlSkin);
  delete xmlSkin;
}
 
void Skin::saveAsXmlFile(const File& file)
{
  XmlElement *xmlSkin = getAsXml();
  saveXmlToFile(*xmlSkin, file);
  delete xmlSkin;
}

void Skin::addFontToXmlAndSerialise(XmlElement& xml, const Font& theFont, 
                                    const String& attributeName)
{
  String fontName = retainOnlyLettersAndNumbers(theFont.getTypefaceName());
  xml.setAttribute(attributeName, fontName);
  xml.setAttribute(attributeName + "Height", theFont.getHeight());

  String fontPath = "/Skins/Fonts/" + fontName + ".jff";
  File fontFile   = File(getApplicationDirectoryAsString() + fontPath);
  if( !fontFile.existsAsFile() )
  {
    fontFile.create();
    FileOutputStream outStream(fontFile);
    CustomTypeface ctf;
    ctf.addGlyphsFromOtherTypeface(*theFont.getTypeface(), 0, 256);
    ctf.writeToStream(outStream);
    outStream.flush();
  }
}

void Skin::retrieveFontFromXml(XmlElement& xml, Font& theFont, String& attributeName)
{
  String fontName = xml.getStringAttribute(attributeName, Font::getDefaultSansSerifFontName());
  String fontPath = "/Skins/Fonts/" + fontName + ".jff";
  File fontFile   = File(getApplicationDirectoryAsString() + fontPath);
  if( fontFile.existsAsFile() )
  {
    FileInputStream inStream(fontFile);
    CustomTypeface *ctf = new CustomTypeface(inStream);
    theFont = Font(ctf);
    theFont.setTypefaceName(fontName);
  }
  else
    theFont.setTypefaceName(fontName); // tries to find it among installed typefaces
  theFont.setHeight((float)xml.getDoubleAttribute(attributeName + "Height", 14.0));
}

void Skin::initialize()
{
  backgroundColor          = getGrayValue( 64);
  backgroundHighlightColor = getGrayValue( 96);
  widgetBackgroundColor    = getGrayValue(  0); 
  widgetHandleColor        = getGrayValue( 96);
  outlineColor             = getGrayValue(192);
  outlineHighlightColor    = getGrayValue(255);
  textColor                = getGrayValue(192);
  textHighlightColor       = getGrayValue(255);
  markerColor              = getGrayValue(255);
  plotBackgroundColor      = getGrayValue( 96);
  plotForegroundColor      = getGrayValue(192);


  //widgetFont.setTypefaceName("Accidental Presidency");
  //widgetFont.setTypefaceName("Aurulent Sans"); // readable, simple
  //widgetFont.setTypefaceName("Berlin Email"); // too narrow
  //widgetFont.setTypefaceName("Continuum"); // needs larger size
  //widgetFont.setTypefaceName("Familiar Pro");
  //widgetFont.setTypefaceName("Forgotten Futurist");
  //widgetFont.setTypefaceName("Frederic"); // readable, simple
  //widgetFont.setTypefaceName("Hall Fetica");
  //widgetFont.setTypefaceName("Kimberley"); // readable, stylish
  //widgetFont.setHeight(14.f);
}
