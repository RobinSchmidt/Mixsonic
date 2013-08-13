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


  // \todo getAsXml, setFromXml, loadFromXmlFile, saveAsXmlFile, etc....


  // data:
  Colour backgroundColor, 
         highlightBackgroundColor,         
         widgetBackgroundColor, 
         widgetHandleColor,
         outlineColor, 
         highlightOutlineColor, 
         textColor, 
         highlightTextColor;

  //Typeface headlineTypeFace, widgetTypeface, infoTypeFace, etc... - or maybe use
  //class Font instead of Typeface

  // int widgetOutlineThickness, smallButtonSize, standardButtonSize, bigButtonSize


protected:
  
  /** Initializes the members to default values. */
  virtual void initialize();

  /** Inaccessible constructor. */
  Skin();   

  static Skin *instance; // pointer to the sole instance
  
  JUCE_LEAK_DETECTOR(Skin);
};

#endif
