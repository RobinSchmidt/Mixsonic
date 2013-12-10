#include "MixsonicLookAndFeel.h"

#include "../components/widgets/RScrollBar.h"

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


// we need to move this into RScrollbar::paint in order to have access to the skin:
void MixsonicLookAndFeel::drawScrollbarButton(Graphics& g, ScrollBar& scrollbar, int width, 
  int height, int buttonDirection, bool isScrollbarVertical, bool isMouseOverButton, 
  bool isButtonDown)
{
  RScrollBar *s = dynamic_cast<RScrollBar*> (&scrollbar);
  if( s == nullptr )
  {
    LookAndFeel::drawScrollbarButton(g, scrollbar, width, height, buttonDirection, 
      isScrollbarVertical, isMouseOverButton, isButtonDown);
    return;
  }

  Path p;
  addTriangleToPath(p, (float)width, (float)height, buttonDirection);

  if(isButtonDown)
    g.setColour(scrollbar.findColour(ScrollBar::thumbColourId).contrasting (0.2f));
  else
    g.setColour(scrollbar.findColour(ScrollBar::thumbColourId));
  g.fillPath(p);


  // what's this?:
  g.setColour(Colour(0x80000000));
  g.strokePath(p, PathStrokeType (0.5f));
}




void MixsonicLookAndFeel::drawScrollbar(Graphics& g, ScrollBar& scrollbar, int x, int y, int width, 
  int height, bool isScrollbarVertical, int thumbStartPosition, int thumbSize, bool isMouseOver,  
  bool isMouseDown)
{
  RScrollBar *s = dynamic_cast<RScrollBar*> (&scrollbar);
  if( s == nullptr )
  {
    LookAndFeel::drawScrollbar(g, scrollbar, x, y, width, height, isScrollbarVertical, 
      thumbStartPosition, thumbSize, isMouseOver, isMouseDown);
    return;
  }

  g.fillAll(scrollbar.findColour(ScrollBar::backgroundColourId));

  Path slotPath, thumbPath;

  const float slotIndent = jmin (width, height) > 15 ? 1.0f : 0.0f;
  const float slotIndentx2 = slotIndent * 2.0f;
  const float thumbIndent = slotIndent + 1.0f;
  const float thumbIndentx2 = thumbIndent * 2.0f;

  float gx1 = 0.0f, gy1 = 0.0f, gx2 = 0.0f, gy2 = 0.0f;

  if (isScrollbarVertical)
  {
    slotPath.addRoundedRectangle (x + slotIndent,
      y + slotIndent,
      width - slotIndentx2,
      height - slotIndentx2,
      (width - slotIndentx2) * 0.5f);

    if (thumbSize > 0)
      thumbPath.addRoundedRectangle (x + thumbIndent,
      thumbStartPosition + thumbIndent,
      width - thumbIndentx2,
      thumbSize - thumbIndentx2,
      (width - thumbIndentx2) * 0.5f);
    gx1 = (float) x;
    gx2 = x + width * 0.7f;
  }
  else
  {
    slotPath.addRoundedRectangle (x + slotIndent,
      y + slotIndent,
      width - slotIndentx2,
      height - slotIndentx2,
      (height - slotIndentx2) * 0.5f);

    if (thumbSize > 0)
      thumbPath.addRoundedRectangle (thumbStartPosition + thumbIndent,
      y + thumbIndent,
      thumbSize - thumbIndentx2,
      height - thumbIndentx2,
      (height - thumbIndentx2) * 0.5f);
    gy1 = (float) y;
    gy2 = y + height * 0.7f;
  }

  const Colour thumbColour (scrollbar.findColour (ScrollBar::thumbColourId));
  Colour trackColour1, trackColour2;

  if (scrollbar.isColourSpecified (ScrollBar::trackColourId)
    || isColourSpecified (ScrollBar::trackColourId))
  {
    trackColour1 = trackColour2 = scrollbar.findColour (ScrollBar::trackColourId);
  }
  else
  {
    trackColour1 = thumbColour.overlaidWith (Colour (0x44000000));
    trackColour2 = thumbColour.overlaidWith (Colour (0x19000000));
  }

  g.setGradientFill (ColourGradient (trackColour1, gx1, gy1,
    trackColour2, gx2, gy2, false));
  g.fillPath (slotPath);

  if (isScrollbarVertical)
  {
    gx1 = x + width * 0.6f;
    gx2 = (float) x + width;
  }
  else
  {
    gy1 = y + height * 0.6f;
    gy2 = (float) y + height;
  }

  g.setGradientFill (ColourGradient (Colours::transparentBlack,gx1, gy1,
    Colour (0x19000000), gx2, gy2, false));
  g.fillPath (slotPath);

  g.setColour (thumbColour);
  g.fillPath (thumbPath);

  g.setGradientFill (ColourGradient (Colour (0x10000000), gx1, gy1,
    Colours::transparentBlack, gx2, gy2, false));

  g.saveState();

  if (isScrollbarVertical)
    g.reduceClipRegion (x + width / 2, y, width, height);
  else
    g.reduceClipRegion (x, y + height / 2, width, height);

  g.fillPath (thumbPath);
  g.restoreState();

  g.setColour (Colour (0x4c000000));
  g.strokePath (thumbPath, PathStrokeType (0.4f));
}

/*
void MixsonicLookAndFeel::drawScrollbar(Graphics& g, ScrollBar& scrollbar, int x, int y, int width, 
  int height, bool isScrollbarVertical, int thumbStartPosition, int thumbSize, bool isMouseOver,  
  bool isMouseDown)
{
  RScrollBar *s = dynamic_cast<RScrollBar*> (&scrollbar);
  if( s == nullptr )
  {
    LookAndFeel::drawScrollbar(g, scrollbar, x, y, width, height, isScrollbarVertical, 
      thumbStartPosition, thumbSize, isMouseOver, isMouseDown);
    return;
  }

  g.fillAll(scrollbar.findColour(ScrollBar::backgroundColourId));

  Path slotPath, thumbPath;

  const float slotIndent = jmin (width, height) > 15 ? 1.0f : 0.0f;
  const float slotIndentx2 = slotIndent * 2.0f;
  const float thumbIndent = slotIndent + 1.0f;
  const float thumbIndentx2 = thumbIndent * 2.0f;

  float gx1 = 0.0f, gy1 = 0.0f, gx2 = 0.0f, gy2 = 0.0f;

  if (isScrollbarVertical)
  {
    slotPath.addRoundedRectangle (x + slotIndent,
      y + slotIndent,
      width - slotIndentx2,
      height - slotIndentx2,
      (width - slotIndentx2) * 0.5f);

    if (thumbSize > 0)
      thumbPath.addRoundedRectangle (x + thumbIndent,
      thumbStartPosition + thumbIndent,
      width - thumbIndentx2,
      thumbSize - thumbIndentx2,
      (width - thumbIndentx2) * 0.5f);
    gx1 = (float) x;
    gx2 = x + width * 0.7f;
  }
  else
  {
    slotPath.addRoundedRectangle (x + slotIndent,
      y + slotIndent,
      width - slotIndentx2,
      height - slotIndentx2,
      (height - slotIndentx2) * 0.5f);

    if (thumbSize > 0)
      thumbPath.addRoundedRectangle (thumbStartPosition + thumbIndent,
      y + thumbIndent,
      thumbSize - thumbIndentx2,
      height - thumbIndentx2,
      (height - thumbIndentx2) * 0.5f);
    gy1 = (float) y;
    gy2 = y + height * 0.7f;
  }

  const Colour thumbColour (scrollbar.findColour (ScrollBar::thumbColourId));
  Colour trackColour1, trackColour2;

  if (scrollbar.isColourSpecified (ScrollBar::trackColourId)
    || isColourSpecified (ScrollBar::trackColourId))
  {
    trackColour1 = trackColour2 = scrollbar.findColour (ScrollBar::trackColourId);
  }
  else
  {
    trackColour1 = thumbColour.overlaidWith (Colour (0x44000000));
    trackColour2 = thumbColour.overlaidWith (Colour (0x19000000));
  }

  g.setGradientFill (ColourGradient (trackColour1, gx1, gy1,
    trackColour2, gx2, gy2, false));
  g.fillPath (slotPath);

  if (isScrollbarVertical)
  {
    gx1 = x + width * 0.6f;
    gx2 = (float) x + width;
  }
  else
  {
    gy1 = y + height * 0.6f;
    gy2 = (float) y + height;
  }

  g.setGradientFill (ColourGradient (Colours::transparentBlack,gx1, gy1,
    Colour (0x19000000), gx2, gy2, false));
  g.fillPath (slotPath);

  g.setColour (thumbColour);
  g.fillPath (thumbPath);

  g.setGradientFill (ColourGradient (Colour (0x10000000), gx1, gy1,
    Colours::transparentBlack, gx2, gy2, false));

  g.saveState();

  if (isScrollbarVertical)
    g.reduceClipRegion (x + width / 2, y, width, height);
  else
    g.reduceClipRegion (x, y + height / 2, width, height);

  g.fillPath (thumbPath);
  g.restoreState();

  g.setColour (Colour (0x4c000000));
  g.strokePath (thumbPath, PathStrokeType (0.4f));
}
*/