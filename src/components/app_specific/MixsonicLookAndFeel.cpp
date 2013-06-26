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
  if(isItemSelected)
  {
    g.fillAll(highlightBackgroundColor);
    g.setColour(highlightTextColor);
  }
  else
    g.setColour(textColor);
  g.setFont(height * 0.7f);
  g.drawText(filename, 0, 0, width, height, Justification::centredLeft, 1);
}
