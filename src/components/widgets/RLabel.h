#ifndef RLabel_h
#define RLabel_h

#include "RWidget.h"

/**

This class is a juce::Label extended by the functionality of RWidget. 

*/

class RLabel : public Label, public RWidget
{

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  RLabel(const String& componentName = String("RLabel"), 
    const String& labelText = String("RLabelText"));

  /** Destructor. */
  virtual ~RLabel();

  //-----------------------------------------------------------------------------------------------
  // others:

  /** Overrides the mouseEnter callback in order to show the description in the dedicated field 
  when the mouse enters the widget. */
  virtual void mouseEnter(const MouseEvent &e);

  /** Overrides the mouseExit callback in order to make the description disappear when the mouse 
  leaves the widget. */
  virtual void mouseExit(const MouseEvent &e);

protected:
  JUCE_LEAK_DETECTOR(RLabel);
};


/** Subclass of Rlabel which may use different colors, fonts, etc. */
class RHeadlineLabel : public RLabel
{
public:
  RHeadlineLabel(const String& componentName = String("RLabel"), 
    const String& labelText = String("RLabelText"));

protected:
  JUCE_LEAK_DETECTOR(RHeadlineLabel);
};


#endif  
