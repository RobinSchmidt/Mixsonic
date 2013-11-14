#include "MixsonicLookAndFeel.h"

//-------------------------------------------------------------------------------------------------
// overriden drawing functions:

void MixsonicLookAndFeel::drawFileBrowserRow(Graphics& g, 
                                             int width, 
                                             int height, 
                                             const String& filename, 
                                             Image* icon, 
                                             const String& fileSizeDescription, 
                                             const String& fileTimeDescription, 
                                             bool isDirectory, 
                                             bool isItemSelected, 
                                             int itemIndex,
                                             DirectoryContentsDisplayComponent& component)
{
  if( isItemSelected )
  {
    g.fillAll(Skin::getInstance()->backgroundHighlightColor);
    g.setColour(Skin::getInstance()->textHighlightColor);
  }
  else
    g.setColour(Skin::getInstance()->textColor);
  g.setFont(Skin::getInstance()->textFont);
  g.drawText(filename, 0, 0, width, height, Justification::centredLeft, 1);
}
