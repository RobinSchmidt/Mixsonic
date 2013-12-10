#ifndef RWidget_h
#define RWidget_h

#include "../../core/infrastructure/DescribedItem.h"
#include "../../control/Skin.h"

/**

This class serves as base class for various GUI widgets. This class is derived from 
juce::MouseListener and not from juce::Component to make it easier to derive widgets from the 
original JUCE widgets and from RWidget - the juce wdigets are themselves subclasses of Component, 
which would raise problems when deriving from both. Whe you want to write an RWidget which does 
not rely on some juce-widget, you may consider to derive it form RWidgetComponent instead.

*/

class RWidget : public DescribedMouseListener
{

public:

  /** Constructor. */
  RWidget(Skin *skinToUse, const String& newDescription = String("Widget"));

protected:

  /** A pointer to the function which converts a value into a juce::String. */
  String (*stringConversionFunction) (double valueToConvert);

  Skin *skin;

  JUCE_LEAK_DETECTOR(RWidget);

};

#endif  
