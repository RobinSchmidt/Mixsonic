#include "Skin.h"

Skin::Skin()
{
  initialize();
}
  
void Skin::initialize()
{
  initializeColors();
  initializeFonts();
}

void Skin::initializeColors()
{
  backgroundColor                = getGrayValue( 64);
  backgroundColorSemiHighlight   = getGrayValue( 96);
  backgroundColorHighlight       = getGrayValue(128);

  middlegroundColor              = getGrayValue(128);
  middlegroundColorSemiHighlight = getGrayValue(160);
  middlegroundColorHighlight     = getGrayValue(192);

  foregroundColor                = getGrayValue(192);
  foregroundColorSemiHighlight   = getGrayValue(224);
  foregroundColorHighlight       = getGrayValue(255);

  outlineColor                   = getGrayValue(192);
  outlineColorSemiHighlight      = getGrayValue(224);
  outlineColorHighlight          = getGrayValue(255);
}

void Skin::initializeFonts()
{
  /*
  setFont(smallFont,  "SansSerif", 12.f);
  setFont(normalFont, "SansSerif", 14.f);
  setFont(bigFont,    "SansSerif", 17.f);
  */

  setFont(smallFont,  "Kimberley", 12.f);
  setFont(normalFont, "Kimberley", 14.f);
  setFont(bigFont,    "Kimberley", 17.f); // readable, stylish



  // old code - just for reference of the font-names, to be tried:

  //textFont.setTypefaceName("Kimberley"); // readable, stylish
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

void Skin::setFont(Font& fontToSet, const String& typefaceName, float height)
{
  String fontPath = "/Skins/Fonts/" + typefaceName + ".jff";
  File fontFile   = File(getApplicationDirectoryAsString() + fontPath);
  if( fontFile.existsAsFile() )
  {
    FileInputStream inStream(fontFile);
    CustomTypeface *ctf = new CustomTypeface(inStream);
    fontToSet = Font(ctf);
    fontToSet.setTypefaceName(typefaceName);
  }
  else
  {
    fontToSet.setTypefaceName(typefaceName);
    if( fontToSet.getTypeface() == nullptr )
      fontToSet.setTypefaceName(Font::getDefaultSansSerifFontName());
  }
  fontToSet.setHeight(height);

  // mainly for debug:
  Typeface *tf = fontToSet.getTypeface();
  jassert( tf != nullptr );
}

 
void Skin::copyDataFrom(Skin *skin)
{
  backgroundColor                = skin->backgroundColor;
  backgroundColorSemiHighlight   = skin->backgroundColorSemiHighlight;
  backgroundColorHighlight       = skin->backgroundColorHighlight;
  middlegroundColor              = skin->middlegroundColor;
  middlegroundColorSemiHighlight = skin->middlegroundColorSemiHighlight;
  middlegroundColorHighlight     = skin->middlegroundColorHighlight;
  foregroundColor                = skin->foregroundColor;
  foregroundColorSemiHighlight   = skin->foregroundColorSemiHighlight;
  foregroundColorHighlight       = skin->foregroundColorHighlight;
  outlineColor                   = skin->outlineColor;
  outlineColorSemiHighlight      = skin->outlineColorSemiHighlight;
  outlineColorHighlight          = skin->outlineColorHighlight;

  smallFont                      = skin->smallFont;
  normalFont                     = skin->normalFont;
  bigFont                        = skin->bigFont;
}

XmlElement* Skin::getAsXml(const String& xmlElementName)
{
  XmlElement *xmlSkin = new XmlElement(xmlElementName);

  XmlElement *xmlColors = new XmlElement("COLORS");
  xmlSkin->addChildElement(xmlColors);
  xmlColors->setAttribute("background",                backgroundColor.toString());
  xmlColors->setAttribute("backgroundSemiHighlight",   backgroundColorSemiHighlight.toString());
  xmlColors->setAttribute("backgroundHighlight",       backgroundColorHighlight.toString());
  xmlColors->setAttribute("middleground",              middlegroundColor.toString());
  xmlColors->setAttribute("middlegroundSemiHighlight", middlegroundColorSemiHighlight.toString());
  xmlColors->setAttribute("middlegroundHighlight",     middlegroundColorHighlight.toString());
  xmlColors->setAttribute("foreground",                foregroundColor.toString());
  xmlColors->setAttribute("foregroundSemiHighlight",   foregroundColorSemiHighlight.toString());
  xmlColors->setAttribute("foregroundHighlight",       foregroundColorHighlight.toString());
  xmlColors->setAttribute("outline",                   outlineColor.toString());
  xmlColors->setAttribute("outlineSemiHighlight",      outlineColorSemiHighlight.toString());
  xmlColors->setAttribute("outlineHighlight",          outlineColorHighlight.toString());

  XmlElement *xmlFonts = new XmlElement("FONTS");
  xmlSkin->addChildElement(xmlFonts);
  addFontToXmlAndSerialise(*xmlFonts, smallFont,  "smallFont");
  addFontToXmlAndSerialise(*xmlFonts, normalFont, "normalFont");
  addFontToXmlAndSerialise(*xmlFonts, bigFont,    "bigFont");

  // \todo control further aspects, like the gradient colors for the level/pan sliders, outline 
  // thicknesses, images for buttons etc, widget sizes, etc.

  return xmlSkin;
}

void Skin::setFromXml(const XmlElement& xml, bool resetBefore)
{
  if( resetBefore == true )
    initialize();

  XmlElement *child = xml.getChildByName("COLORS");
  if( child != nullptr )
  {
    // if no highlight-color is defined, we use the normal color for highlighted items as well and 
    // if no semi-highlight color is defined, we use the highlight color for semi-highlighting:

    backgroundColor = Colour::fromString(
      child->getStringAttribute("background", backgroundColor.toString()));
    backgroundColorHighlight = Colour::fromString(
      child->getStringAttribute("backgroundHighlight", backgroundColor.toString()));
    backgroundColorSemiHighlight = Colour::fromString(
      child->getStringAttribute("backgroundSemiHighlight", backgroundColorHighlight.toString()));

    middlegroundColor = Colour::fromString(
      child->getStringAttribute("middleground", middlegroundColor.toString()));
    middlegroundColorHighlight = Colour::fromString(
      child->getStringAttribute("middlegroundHighlight", middlegroundColor.toString()));
    middlegroundColorSemiHighlight = Colour::fromString(
      child->getStringAttribute("middlegroundSemiHighlight", middlegroundColorHighlight.toString()));

    foregroundColor = Colour::fromString(
      child->getStringAttribute("foreground", foregroundColor.toString()));
    foregroundColorHighlight = Colour::fromString(
      child->getStringAttribute("foregroundHighlight", foregroundColor.toString()));
    foregroundColorSemiHighlight = Colour::fromString(
      child->getStringAttribute("foregroundSemiHighlight", foregroundColorHighlight.toString()));

    outlineColor = Colour::fromString(
      child->getStringAttribute("outline", outlineColor.toString()));
    outlineColorHighlight = Colour::fromString(
      child->getStringAttribute("outlineHighlight", outlineColor.toString()));
    outlineColorSemiHighlight = Colour::fromString(
      child->getStringAttribute("outlineSemiHighlight", outlineColorHighlight.toString()));

    // maybe this can be done more elegantly (with less code copy/paste)
  }

  child = xml.getChildByName("FONTS");
  if( child != nullptr )
  {
    retrieveFontFromXml(*child, smallFont,  String("smallFont"));
    retrieveFontFromXml(*child, normalFont, String("normalFont"));
    retrieveFontFromXml(*child, bigFont,    String("bigFont"));
      // maybe get rid of retrieveFontFromXml
  }

  // \todo retrieve sizes, graphics, etc.
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
  String fontName   = xml.getStringAttribute(attributeName, Font::getDefaultSansSerifFontName());
  float  fontHeight = (float)xml.getDoubleAttribute(attributeName + "Height", 14.0);
  setFont(theFont, fontName, fontHeight);
}

//=================================================================================================

SectionSkin::SectionSkin()
{
  initialize();
}
  
void SectionSkin::initialize()
{
  Skin::initialize();
  widgetSkin.initialize();
  labelSkin.initialize();  
  plotSkin.initialize();
}

void SectionSkin::copyDataFrom(SectionSkin *skin)
{
  Skin::copyDataFrom(skin);
  widgetSkin.copyDataFrom(&skin->widgetSkin);
  labelSkin.copyDataFrom(&skin->labelSkin);
  plotSkin.copyDataFrom(&skin->plotSkin);
}

XmlElement* SectionSkin::getAsXml(const String& xmlElementName)
{
  XmlElement *xmlSkin = Skin::getAsXml(xmlElementName);
  xmlSkin->addChildElement(widgetSkin.getAsXml("WIDGETS"));
  xmlSkin->addChildElement(labelSkin.getAsXml("LABELS"));
  xmlSkin->addChildElement(plotSkin.getAsXml("PLOTS"));
  return xmlSkin;
}

void SectionSkin::setFromXml(const XmlElement& xml, bool resetBefore)
{
  if( resetBefore == true )
    initialize();

  Skin::setFromXml(xml, resetBefore);
  XmlElement *child;
  child = xml.getChildByName("WIDGETS"); 
  if( child != nullptr )  
    widgetSkin.setFromXml(*child, resetBefore);
  child = xml.getChildByName("LABELS"); 
  if( child != nullptr )  
    labelSkin.setFromXml(*child, resetBefore);
  child = xml.getChildByName("PLOTS"); 
  if( child != nullptr )  
    plotSkin.setFromXml(*child, resetBefore);
  // try to get rid of the code duplication
}

//=================================================================================================

ApplicationSkin::ApplicationSkin()
{
  appName = "Application";
}

XmlElement* ApplicationSkin::getAsXml()
{
  XmlElement *xmlSkin = SectionSkin::getAsXml(appName.toUpperCase() + "SKIN"); 
  for(int i = 0; i < sectionNames.size(); i++)
    xmlSkin->addChildElement(sectionSkins[i]->getAsXml(sectionNames[i].toUpperCase()));
  return xmlSkin;
}

void ApplicationSkin::setFromXml(const XmlElement& xml)
{
  SectionSkin::setFromXml(xml, true);
  XmlElement *child;
  for(int i = 0; i < sectionNames.size(); i++)
  {
    sectionSkins[i]->copyDataFrom(this);
    child = xml.getChildByName(sectionNames[i].toUpperCase());
    if( child != nullptr )
      sectionSkins[i]->setFromXml(*child, false);
  }
}

void ApplicationSkin::loadFromXmlFile(const File& file)
{
  XmlElement* xmlSkin = loadXmlFromFile(file);
  if( xmlSkin != nullptr )
    setFromXml(*xmlSkin);
  delete xmlSkin;
}
 
void ApplicationSkin::saveAsXmlFile(const File& file)
{
  XmlElement *xmlSkin = getAsXml();
  saveXmlToFile(*xmlSkin, file);
  delete xmlSkin;
}

SectionSkin* ApplicationSkin::getSectionSkin(const String& sectionName)
{
  for(int i = 0; i < sectionNames.size(); i++)
  {
    if( sectionNames[i] == sectionName )
      return sectionSkins[i];
  }
  return this;
}

void ApplicationSkin::addSectionSkin(const String& name)
{
  sectionNames.add(name);
  sectionSkins.add(new SectionSkin());
}

//=================================================================================================

MixsonicSkin* MixsonicSkin::instance = nullptr;

MixsonicSkin::MixsonicSkin() 
{
  initialize();
}   

MixsonicSkin* MixsonicSkin::getInstance()
{
  if( instance == nullptr )
    instance = new MixsonicSkin();
  return instance;
}

void MixsonicSkin::releaseInstance()
{
  delete instance;
  instance = nullptr;
}

void MixsonicSkin::initialize()
{
  appName = "Mixsonic";
  addSectionSkin("Browser");
  addSectionSkin("Arranger");
  addSectionSkin("Plugin");
  addSectionSkin("Dialog");
}
