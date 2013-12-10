#ifndef RScrollBar_h
#define RScrollBar_h

#include "RWidget.h"

/**

This class is a juce::ScrollBar extended by the functionality of RWidget. 

*/

class RScrollBar : public ScrollBar, public RWidget
{

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  RScrollBar(bool vertical, bool buttonsVisible, Skin *skinToUse);

  /** Destructor. */
  //virtual ~RScrollBar();

  //-----------------------------------------------------------------------------------------------
  // others:


  virtual void paint(Graphics& g);


  // \todo override paint


protected:
  JUCE_LEAK_DETECTOR(RScrollBar);
};

#endif  
