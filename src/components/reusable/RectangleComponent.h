#ifndef RectangleComponent_h
#define RectangleComponent_h

#include "../../libs/JuceIncludes.h"

/**

This class is component which has the sole purpose to draw itself as an rectangle filled with some
color and draw an outline with another color.

*/

class RectangleComponent : public Component
{  

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. Initializes the colors to be used for drawing the rectangle. */
  RectangleComponent(const Colour &newFillColor = Colours::white, 
    const Colour &newOutlineColor = Colours::black);

  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Sets the color with which the rectangle will be filled. */
  virtual void setFillColor(const Colour &newFillColor);

  /** Sets the color for the outline of the rectangle. */
  virtual void setOutlineColor(const Colour &newOutlineColor);

  //-----------------------------------------------------------------------------------------------
  // others:

  /** Paints the rectangle. */
  virtual void paint(Graphics &g);


protected:

  /** The colours to use. */
  Colour fillColor, outlineColor;

  // float outlineThickness; // maybe add later, if desired

  JUCE_LEAK_DETECTOR(RectangleComponent);
};

#endif  