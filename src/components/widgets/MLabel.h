#ifndef MLabel_h
#define MLabel_h

#include "MWidget.h"

/**

This class is a juce::Label extended by the functionality of MWidget. 

\todo: replace with RTextField

*/

class MLabel : public Label, public MWidget
{

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  MLabel(Skin *skinToUse, const String& componentName = String("MLabel"), 
    const String& labelText = String("RLabelText"));

  /** Destructor. */
  virtual ~MLabel();

  //-----------------------------------------------------------------------------------------------
  // others:

  /** Overrides the mouseEnter callback in order to show the description in the dedicated field 
  when the mouse enters the widget. */
  virtual void mouseEnter(const MouseEvent &e);

  /** Overrides the mouseExit callback in order to make the description disappear when the mouse 
  leaves the widget. */
  virtual void mouseExit(const MouseEvent &e);

protected:
  JUCE_LEAK_DETECTOR(MLabel);
};


/** Subclass of Rlabel which may use different colors, fonts, etc. 
maybe, get rid of this subclass
*/
class MHeadlineLabel : public MLabel
{
public:
  MHeadlineLabel(Skin *skinToUse, const String& componentName = String("MLabel"), 
    const String& labelText = String("RLabelText"));

protected:
  JUCE_LEAK_DETECTOR(MHeadlineLabel);
};


#endif  
