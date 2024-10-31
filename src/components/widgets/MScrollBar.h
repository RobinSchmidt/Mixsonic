#ifndef MScrollBar_h
#define MScrollBar_h

#include "MWidget.h"

/**

This class is a juce::ScrollBar extended by the functionality of MWidget. 

*/

class MScrollBar : public ScrollBar, public MWidget
{

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  MScrollBar(bool vertical, bool buttonsVisible, Skin *skinToUse);

  //-----------------------------------------------------------------------------------------------
  // others:

  virtual void paint(Graphics& g);

protected:
  JUCE_LEAK_DETECTOR(MScrollBar);
};

#endif  
