#include "Skin.h"

Skin* Skin::instance        = nullptr;
//File  Skin::defaultSkinFile = File::nonexistent; 

Skin::Skin() 
{
  initialize();

  // load default skin from file (if present):
  lastUsedSkinFile = File(getApplicationDirectoryAsString() + "/Skins/LastUsed.xml");
  loadFromXmlFile(lastUsedSkinFile);
    // we should probably have a skinFile field in the preferences file which contains the name
    // of the skin to be loaded
}   

Skin* Skin::getInstance()
{
  if( instance == nullptr )
    instance = new Skin();
  return instance;
}

void Skin::releaseInstance()
{
  // \todo - maybe save the current skin into the default skin file
  // maybe it should be named LastUsed or something, and we should not do this in releaseInstance
  if( instance != nullptr )
    instance->saveAsXmlFile(instance->lastUsedSkinFile);

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

  addFontToXmlAndSerialise(*xmlFonts, textFont, "textFont");
  xmlFonts->setAttribute("textFontHeight", textFont.getHeight());

  addFontToXmlAndSerialise(*xmlFonts, widgetFont, "widgetFont");
  xmlFonts->setAttribute("widgetFontHeight", widgetFont.getHeight());

  addFontToXmlAndSerialise(*xmlFonts, headlineFont, "headlineFont");
  xmlFonts->setAttribute("headlineFontHeight", headlineFont.getHeight());

  xmlSkin->addChildElement(xmlFonts);



  //XmlElement *xmlSizes = new XmlElement("SIZES");
  //...
  //xmlSkin->addChildElement(xmlSizes);

  //XmlElement *xmlImages = new XmlElement("IMAGES");
  //...
  //xmlSkin->addChildElement(xmlImages);


  // control further aspects, like the gradient colors for the level/pan sliders, outline 
  // thicknesses, etc.


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

  // \todo retrieve sizes, fonts, images, etc.
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
  String fontName = retainOnlyAlhpanumericCharacters(theFont.getTypefaceName());
  xml.setAttribute(attributeName, fontName);

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


  //widgetFont.setTypefaceName("Accidental Presidency");
  //widgetFont.setTypefaceName("Aurulent Sans"); // readable, simple
  //widgetFont.setTypefaceName("Berlin Email"); // too narrow
  //widgetFont.setTypefaceName("Familiar Pro");
  //widgetFont.setTypefaceName("Forgotten Futurist");
  //widgetFont.setTypefaceName("Frederic"); // readable, simple
  //widgetFont.setTypefaceName("Hall Fetica");

  widgetFont.setTypefaceName("Kimberley"); // readable, stylish
  //widgetFont.setHeight(16.f);
  widgetFont.setHeight(14.f);
  //widgetFont.setSizeAndStyle(16.f, 0, 1.f, 1.f),

  //widgetFont.setTypefaceName("Nonserif");


  //widgetFont.setTypefaceName("Segoe Script");
  //widgetFont.setTypefaceName(Font::getDefaultSansSerifFontName());
  //widgetFont.setTypefaceName(Font::getDefaultSerifFontName());
  //widgetFont.setTypefaceName(Font::getDefaultMonospacedFontName());

}

//
