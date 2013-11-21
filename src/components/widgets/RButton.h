#ifndef RButton_h
#define RButton_h

#include "RWidget.h"

/**

This class is a juce::ToggleButton extended by the functionality of RWidget. 

*/

class RButton : public ToggleButton, public RWidget
{

public:

  enum buttonSymbols
  {
    NO_SYMBOL = 0,
    PLUS,
    MINUS,
    PLAY,
    SKIP_FORWARD,
    SKIP_BACK,
    MUTE,
    LOOP,

    NUM_SYMBOLS
  };

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructs a button with a symbol. */
  RButton(Skin *skinToUse, int newSymbolIndex);    

  /** Constructs a button with text. */
  RButton(Skin *skinToUse, const String& componentName = String("RButton"));

  /** Destructor. */
  virtual ~RButton();

  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Chooses a new symbol for this button. */
  virtual void setSymbolIndex(int newSymbolIndex);

  //-----------------------------------------------------------------------------------------------
  // others:

  /** Triggers loading of a color-scheme from a file ColorScheme.xml - if the file doesn't exist,
  it will fall back to a default color-scheme. */
  virtual void loadColorScheme();

  /** Overrides the mouseEnter callback in order to show the description in the dedicated field 
  when the mouse enters the widget. */
  virtual void mouseEnter(const MouseEvent &e);

  /** Overrides the mouseExit callback in order to make the description disappear when the mouse 
  leaves the widget. */
  virtual void mouseExit(const MouseEvent &e);

  /** Paints the button. */
  virtual void paint(Graphics &g);

  //===============================================================================================
  juce_UseDebuggingNewOperator;

protected:

  /** Paints the button in simple 2D way */
  virtual void paint2D(Graphics &g) const;

  /** Paints the button in a pseudo-3D way */
  virtual void paintPseudo3D(Graphics &g) const;

  /** Draws the symbol onto the button (if any). */
  virtual void drawSymbol(Graphics &g) const;

  int symbolIndex;

  // color-scheme management:
  static Colour backgroundColour, outlineColour, textColour, onStateColour, offStateColour;
  static int instanceCounter;

};

#endif  
