#ifndef RWidget_h
#define RWidget_h

#include "../../core/infrastructure/DescribedItem.h"

/**

This class serves as base class for various GUI widgets. This class is derived from 
juce::MouseListener and not from juce::Component to make it easier to derive widgets from the 
original JUCE widgets and from RWidget - the juce wdigets are themselves subclasses of Component, 
which would raise problems when deriving from both. Whe you want to write an RWidget which does 
not rely on some juce-widget, you may consider to derive it form RWidgetComponent instead.

*/

class RWidget : public DescribedItem, public ChangeBroadcaster, virtual public MouseListener
{

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  RWidget(const String& newDescription = String("some widget"));   

  /** Destructor. */
  virtual ~RWidget(); 

  //-----------------------------------------------------------------------------------------------
  // callbacks:

  /** Overrides the mouseEnter callback in order to show the description in the dedicated field 
  when the mouse enters the widget. */
  virtual void mouseEnter(const MouseEvent &e);

  /** Overrides the mouseExit callback in order to make the description disappear when the mouse 
  leaves the widget. */
  virtual void mouseExit(const MouseEvent &e);

  //===============================================================================================
  juce_UseDebuggingNewOperator;

protected:

  /** A pointer to the function which converts a value into a juce::String. */
  String (*stringConversionFunction) (double valueToConvert);

};

#endif  
