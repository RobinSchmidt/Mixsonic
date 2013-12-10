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


  // \todo - later, we may want to implement these functions directlyin RScrollBar, bypassing the
  // LookAndFeel mechanism:

  /** Draws one of the buttons on a scrollbar.
  @param g                    the context to draw into
  @param scrollbar            the bar itself
  @param width                the width of the button
  @param height               the height of the button
  @param buttonDirection      the direction of the button, where 0 = up, 1 = right, 2 = down, 3 = left
  @param isScrollbarVertical  true if it's a vertical bar, false if horizontal
  @param isMouseOverButton    whether the mouse is currently over the button (also true if it's held down)
  @param isButtonDown         whether the mouse button's held down */
  virtual void drawScrollbarButton(Graphics& g, ScrollBar& scrollbar, int width, int height, 
    int buttonDirection, bool isScrollbarVertical, bool isMouseOverButton, bool isButtonDown);

  /** Draws the thumb area of a scrollbar.
  @param g                    the context to draw into
  @param scrollbar            the bar itself
  @param x                    the x position of the left edge of the thumb area to draw in
  @param y                    the y position of the top edge of the thumb area to draw in
  @param width                the width of the thumb area to draw in
  @param height               the height of the thumb area to draw in
  @param isScrollbarVertical  true if it's a vertical bar, false if horizontal
  @param thumbStartPosition   for vertical bars, the y co-ordinate of the top of the
                              thumb, or its x position for horizontal bars
  @param thumbSize            for vertical bars, the height of the thumb, or its width for
                              horizontal bars. This may be 0 if the thumb shouldn't be drawn.
  @param isMouseOver          whether the mouse is over the thumb area, also true if the mouse is
                              currently dragging the thumb
  @param isMouseDown          whether the mouse is currently dragging the scrollbar  */
  virtual void drawScrollbar(Graphics& g, ScrollBar& scrollbar, int x, int y, int width, 
    int height, bool isScrollbarVertical, int thumbStartPosition, int thumbSize, bool isMouseOver,
    bool isMouseDown);

};

#endif  