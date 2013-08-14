#ifndef Skin_h
#define Skin_h

#include "../../libs/JuceIncludes.h"
#include "../../core/infrastructure/GlobalFunctions.h"


/** A class for storing info about the appearance of the graphical user interface. This class 
implements the singleton pattern. 

\todo include bitmap loading, maybe to test the functionality, we can use the UI kits from here:
http://www.kvraudio.com/forum/viewtopic.php?t=373551
*/

class Skin
{

public:

  /** Function to return the pointer to the sole instance of the singleton, possibly allocating it,
  if this hasn't been done yet. */
  static Skin* getInstance();

  /** Function to eventually delete the singleton instance. */
  static void releaseInstance();

  /** Returns an XML description of the skin. */
  XmlElement* getAsXml();

  /** Sets up the skin from an XML description. */
  void setFromXml(const XmlElement& xml);

  /** Sets up the skin from an XML file. */
  void loadFromXmlFile(const File& file);

  /** Saves an XML description of the skin into a file.  */
  void saveAsXmlFile(const File& file);

  /** Adds an attribute for a font to the given XmlElement which stores the name of the font and 
  also creates a corresponding font-file in the appropriate font folder, if such a file is not
  already there for the font in question. */
  static void addFontToXmlAndSerialise(XmlElement& xml, const Font& theFont, 
    const String& attributeName);



  // data:
  Colour backgroundColor, 
         backgroundHighlightColor,         
         widgetBackgroundColor, 
         widgetHandleColor,
         outlineColor, 
         outlineHighlightColor, 
         textColor, 
         textHighlightColor;
  // clipBackgroundColor, clipForegroundColor - maybe use the more general "plot" instead of 
  // "clip"

  Font textFont, widgetFont, headlineFont;

  // int widgetOutlineThickness, smallButtonSize, standardButtonSize, bigButtonSize

  File lastUsedSkinFile; // maybe get rid of this and have a skinFile filed in the setting file 
                         // instead

protected:
  
  /** Initializes the members to default values. */
  virtual void initialize();

  /** Inaccessible constructor. */
  Skin();   

  static Skin *instance; // pointer to the sole instance
  
  JUCE_LEAK_DETECTOR(Skin);
};

#endif
