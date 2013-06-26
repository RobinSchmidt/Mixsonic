#ifndef MixsonicButton_h
#define MixsonicButton_h

//#include "RWidget.h"
#include "RButton.h"

/**

This is a juce::Button subclass to be used in Mixsonic. You can (and should) pass two Image 
objects to it via setImages() - one for the normal (unpressed) state and one for the pressed 
state.

\todo rename to ImageButton, have also a hoverImage (whcih may point to the same image as the
normalImage

*/

class MixsonicButton  : public RButton
{

public:

  //---------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  MixsonicButton();

  /** Destructor. */
  virtual ~MixsonicButton();

  //---------------------------------------------------------------------------------------------
  // setup:

  virtual void setImages(const Image* newNormalImage, const Image* newPressedImage);

  //---------------------------------------------------------------------------------------------
  // inquiry:

  /** Returns true when the button is in pressed state. */
  virtual bool isPressed();

  //---------------------------------------------------------------------------------------------
  // overrides:

  /** Implements the purely virtual method of the Button baseclass. */
  virtual void paintButton(Graphics& g, bool isMouseOverButton, bool isButtonDown);
    // hmm - this doesn't seem to be a baseclass method. maybe it's a method of LookAndFeel?
    // however, maybe, we should override paint instead

  /** Overrides mouseEnter in order to call the method from the baseclass RWidget (as opposed to 
  Button). */
  //virtual void mouseEnter(const MouseEvent &e);

  /** Overrides mouseExit in order to call the method from the baseclass RWidget (as opposed to 
  Button). */
  //virtual void mouseExit(const MouseEvent &e);

  //=============================================================================================
  juce_UseDebuggingNewOperator

private:

  // the images for the different button states:
  const Image* normalImage;
  const Image* pressedImage;
  const Image* hoverImage;

};

#endif
