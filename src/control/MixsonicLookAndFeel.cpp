#include "MixsonicLookAndFeel.h"

#include "../components/widgets/MScrollBar.h"

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
  /*
  if( isItemSelected )
  {
    g.fillAll(MixsonicSkin::getInstance()->backgroundHighlightColor);
    g.setColour(MixsonicSkin::getInstance()->textHighlightColor);
  }
  else
    g.setColour(MixsonicSkin::getInstance()->textColor);
  g.setFont(MixsonicSkin::getInstance()->textFont);
  */

  // It's a bit dirty to reach for the MixsonicSkin singleton here, because this way, it will 
  // affect all filebrowsers in the whole application (and not only the Browser-section of the GUI). 
  // Currently, there are no other browsers, so we can get away with it. Maybe, we can use colors
  // defined in DirectoryContentsDisplayComponent and set them up in the constructors of our 
  // Browsers - we'll see.
  SectionSkin *skin = MixsonicSkin::getInstance()->getSectionSkin("Browser");
  if( isItemSelected )
  {
    g.fillAll(skin->widgetSkin.backgroundColorHighlight);
    g.setColour(skin->widgetSkin.foregroundColorHighlight);
  }
  else
    g.setColour(skin->widgetSkin.foregroundColor);
  g.setFont(skin->widgetSkin.normalFont);
  g.drawText(filename, 0, 0, width, height, Justification::centredLeft, 1);
}

/*
void addTriangleToPath(Path& p, float w, float h, int direction)
{
  if(direction == 0)
    p.addTriangle(w*0.5f, h*0.2f, w*0.1f, h*0.7f, w*0.9f, h*0.7f);
  else if(direction == 1)
    p.addTriangle(w*0.8f, h*0.5f, w*0.3f, h*0.1f, w*0.3f, h*0.9f);
  else if(direction == 2)
    p.addTriangle(w*0.5f, h*0.8f, w*0.1f, h*0.3f, w*0.9f, h*0.3f);
  else if(direction == 3)
    p.addTriangle(w*0.2f, h*0.5f, w*0.7f, h*0.1f, w*0.7f, h*0.9f);
}
*/
