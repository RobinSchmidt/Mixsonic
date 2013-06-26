#ifndef RectangleComponent_h
#define RectangleComponent_h

#include "../../libs/JuceIncludes.h"

/**

This class is component which has the sole purpose to draw itself as an rectangle filled with some
colour and draw an outline with another colour.

*/

class RectangleComponent : public Component
{  

public:

  /** The of colour IDs to use to change fill and outline colour of the rectangle. */
  enum ColourIds
  {
    fillColourId = 1,
    outlineColourId
  };

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. Initializes the colours to be used for drawing the rectangle. */
  RectangleComponent(const Colour &newFillColour = Colours::white, 
    const Colour &newOutlineColour = Colours::black);

  //-----------------------------------------------------------------------------------------------
  // setup:
                     
  /** Sets the colour either for the filling or for the outline. @see ColourIds */
  virtual void setColour(const int  colourId,  const Colour &colour);

  //-----------------------------------------------------------------------------------------------
  // others:

  /** Paints the rectangle. */
  virtual void paint(Graphics &g);

  //===============================================================================================
  juce_UseDebuggingNewOperator;

protected:

  /** The colours to use. */
  Colour fillColour, outlineColour;

};

#endif  