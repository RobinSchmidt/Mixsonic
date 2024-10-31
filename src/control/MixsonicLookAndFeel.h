#ifndef MixsonicLookAndFeel_h
#define MixsonicLookAndFeel_h

#include "../../control/Skin.h"

/**

This class implements the custom LookAndFeel subclass for Mixsonic.

*/

class MixsonicLookAndFeel : public LookAndFeel
{  

public:

  //-----------------------------------------------------------------------------------------------
  // overriden drawing functions:
    
  virtual void drawFileBrowserRow(Graphics& g, int width, int height, const String& filename, 
    Image* icon, const String& fileSizeDescription, const String& fileTimeDescription, 
    bool isDirectory, bool isItemSelected, int itemIndex,
    DirectoryContentsDisplayComponent& component);

};

#endif  