#ifndef HalfTransparentButton_h
#define HalfTransparentButton_h

#include "MixsonicButton.h"

/**

This class is a MButton which is drawn half-transparently.

*/

class HalfTransparentButton : public TextButton, public DescribedMouseListener /*, public MWidget*/
{

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructs a button with text. */
  HalfTransparentButton(/*Skin *skinToUse, */const String& buttonName);

  /** Destructor. */
  virtual ~HalfTransparentButton();

  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Sets the transparency of this button. */
  virtual void setTransparency(float newTransparency);

  //-----------------------------------------------------------------------------------------------
  // others:

  /** Paints the button. */
  virtual void paint(Graphics &g);

  //===============================================================================================
  juce_UseDebuggingNewOperator;

protected:

  // color management:
  static Colour onStateColour, offStateColour;
  float  transparency;

};

#endif  
