#ifndef Skin_h
#define Skin_h

#include "../../libs/JuceIncludes.h"
#include "../../core/infrastructure/GlobalFunctions.h"

/** A class for storing info about the appearance of a part of a graphical user interface, 
including colors, fonts and graphics. This "part" is quite generic - it can be a widget, a plot or
even an entire (sub)section of a GUI. In different kinds of parts, the meanings of "middleground" 
and "foreground" may apply to different aspects of the part, whichever meaning seems most 
appropriate intuitively. For example, in a slider, the middleground will be the slider-handle and 
the foregorund will be the text (written above the handle), etc. In a plot with 
manipulation-handles, the middleground will be the graph and the foreground will be the handles.

\todo include bitmap and/or .svg loading, maybe to test the functionality, we can use the UI kits 
from here:
http://www.kvraudio.com/forum/viewtopic.php?t=373551

*/

class Skin
{

public:
  
  /** Constructor */  
  Skin();   

  /** Initializes the members to default values. */
  virtual void initialize();

  virtual void initializeColors();

  virtual void initializeFonts();


  /** First, i checks for a suitable fontfile (.jff) in the font directory, if so, it loads the 
  font from there. If not, it looks into the installed fonts - if it finds a suitabel typeface, it
  will load it from there (todo: create the fontfile then). If that also fails, it just leaves the
  typeface as is. Then, it set up the height. */
  virtual void setFont(Font& fontToSet, const String& typefaceName, float height);


  virtual void copyDataFrom(Skin *skin);


  /** Returns an XML description of the skin. */
  virtual XmlElement* getAsXml(const String& xmlElementName = String("SKIN"));

  /** Sets up the skin from an XML description. If "resetBefore" is true, all members will be set 
  to default values before the xml is parsed which has the effect that for missing attributes
  in the xml-element, default-values will be used. If no reset takes place, the current values
  will persist when an attribute is missing in the xml-element */
  virtual void setFromXml(const XmlElement& xml, bool resetBefore);

  /** Adds an attribute for a font to the given XmlElement which stores the name of the font and 
  also creates a corresponding font-file in the appropriate font folder, if such a file is not
  already there for the font in question. */
  static void addFontToXmlAndSerialise(XmlElement& xml, const Font& theFont, 
    const String& attributeName);

  /** Assigns theFont to a font that is retrieved from the passed xml-element by the 
  attributeName. The value of the attribute with name "attributeName" should be a name of some 
  font which either exists as a .jff file in [ApplicationDirectory]/Skins/Fonts or is a name of an 
  installed font. */
  void retrieveFontFromXml(XmlElement& xml, Font& theFont, String& attributeName);



  // data:
  Colour backgroundColor,   backgroundColorSemiHighlight,   backgroundColorHighlight,
         middlegroundColor, middlegroundColorSemiHighlight, middlegroundColorHighlight,
         foregroundColor,   foregroundColorSemiHighlight,   foregroundColorHighlight,
         outlineColor,      outlineColorSemiHighlight,      outlineColorHighlight;

  Font smallFont, normalFont, bigFont;

  int outlineThickness; // not yet used everywhere

  //Font normalFont, strongFont, weakFont;
    // maybe rename to emphasized and deemphasizedFont
    // or smallFont, normalFont, bigFont


  // maybe factor out a class ColorTriple with members: Color normal, semiHighlight, highlight

  // in widgets: use middleground for slider-handles, foreground for slider text

  // \todo maybe have 2 corresponding arrays with graphics (juce::Drawable) and graphics-names
  // the graphics can be loaded from .png or .svg files

  // maybe have sizes like outlineThickness, smallButtonSize, normalButtonSize, bigButtonSize,
  // scrollbarWidth, sliderWidth, ...

  JUCE_LEAK_DETECTOR(Skin);
};

//=================================================================================================

/** A class for storing info about the appearance of (a section of) a graphical user interface */

class SectionSkin : public Skin
{

public:

  /** Constructor */  
  SectionSkin();  

  /** Initializes the members to default values. */
  virtual void initialize();


  virtual void copyDataFrom(SectionSkin *skin);

  /** Returns an XML description of the skin. */
  virtual XmlElement* getAsXml(const String& xmlElementName = String("SECTIONSKIN"));

  /** Sets up the skin from an XML description. */
  virtual void setFromXml(const XmlElement& xml, bool resetBefore);



  // data:
  Skin widgetSkin, labelSkin, plotSkin;
  
  JUCE_LEAK_DETECTOR(SectionSkin);
};

//=================================================================================================

/** This class holds the Skins for all the sections of the GUI of an application and provides means 
to load and store the whole set of (section)skins from/to and .xml file. The class is an abstract 
baseclass for skins for specific applications. These 
subclasses are supposed to override the initialize method which should set up the name of the 
application and the names and skins for the various GUI sections. */

class ApplicationSkin : public SectionSkin
{

public:

  /** Constructor */  
  ApplicationSkin();   


    
  virtual void initialize();

  /** Returns an XML description of the skin. */
  XmlElement* getAsXml();

  /** Sets up the skin from an XML description. */
  void setFromXml(const XmlElement& xml);

  /** Sets up the skin from an XML file. */
  void loadFromXmlFile(const File& file);

  /** Saves an XML description of the skin into a file.  */
  void saveAsXmlFile(const File& file);

  /** Returns a pointer to the skin for the section with given name or, if the name is not found, a
  fallback-skin. */
  SectionSkin* getSectionSkin(const String& sectionName);

  /** Adds an entry with given name to the sectionNames array and a corresponding entry to the 
  sectionSkins array. */
  void addSectionSkin(const String& name);

protected:
  
  // data:
  String appName;
  Array<String> sectionNames;
  //Array<SectionSkin> sectionSkins;
  OwnedArray<SectionSkin> sectionSkins;

  JUCE_LEAK_DETECTOR(ApplicationSkin);
};

//=================================================================================================

/** Subclass of ApplicationSkin for the Micsonic application (move to another file). It implements
the singleton pattern.

\todo maybe, the singleton pattern is not required anymore, we may just let the application (or its 
toplevel component) have a member of the class.

*/

class MixsonicSkin : public ApplicationSkin
{

public:

  /** Function to return the pointer to the sole instance of the singleton, possibly allocating it,
  if this hasn't been done yet. */
  static MixsonicSkin* getInstance();

  /** Function to eventually delete the singleton instance. */
  static void releaseInstance();

protected:

  //virtual void initialize();

  /** Inaccessible constructor. */
  MixsonicSkin();   

  static MixsonicSkin *instance; // pointer to the sole instance

  JUCE_LEAK_DETECTOR(MixsonicSkin);
};

#endif
