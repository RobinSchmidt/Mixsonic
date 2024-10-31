#ifndef MComboBox_h
#define MComboBox_h

#include "MWidget.h"

/**

This class is a juce::ComboBox extended by the functionality of MWidget. 

*/

class MComboBox : public ComboBox, public MWidget
{

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  MComboBox(Skin *skinToUse, const String& componentName = String("MComboBox"));

  /** Destructor. */
  virtual ~MComboBox();

  //-----------------------------------------------------------------------------------------------
  // others:

  /** Triggers loading of a color-scheme from a file ColorScheme.xml - if the file doesn't exist,
  it will do nothing. */
  virtual void loadColorScheme();

  /** Overrides the mouseEnter callback in order to show the description in the dedicated field 
  when the mouse enters the widget. */
  virtual void mouseEnter(const MouseEvent &e);

  /** Overrides the mouseExit callback in order to make the description disappear when the mouse 
  leaves the widget. */
  virtual void mouseExit(const MouseEvent &e);

  //===============================================================================================
  juce_UseDebuggingNewOperator;

protected:

  // color-scheme management:
  static Colour backgroundColour, outlineColour, textColour, buttonColour, arrowColour;
  static int instanceCounter; //what is this good for?

};

#endif  
