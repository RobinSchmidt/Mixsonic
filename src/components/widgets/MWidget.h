#ifndef MWidget_h
#define MWidget_h

#include "../../core/infrastructure/DescribedItem.h"
#include "../../control/Skin.h"

/**

This class serves as base class for various GUI widgets. This class is derived from 
juce::MouseListener and not from juce::Component to make it easier to derive widgets from the 
original JUCE widgets and from MWidget - the juce wdigets are themselves subclasses of Component, 
which would raise problems when deriving from both. Whe you want to write an MWidget which does 
not rely on some juce-widget, you may consider to derive it form RWidgetComponent instead.

we don't derive this class from juce::Compnent because currently all our widgets are subclasses of 
some juce widget class which are already subclasses of Component. todo: derive MWidget from 
Component and re-implement the actual widget classes

get rid of this class - use RWidget2 instead (and then rename RWidget2 into MWidget later)

*/

class MWidget : public DescribedMouseListener
{

public:

  /** Constructor. */
  MWidget(Skin *skinToUse, const String& newDescription = String("Widget"));

protected:

  /** A pointer to the function which converts a value into a juce::String. */
  String (*stringConversionFunction) (double valueToConvert);

  Skin *skin;

  JUCE_LEAK_DETECTOR(MWidget);

};

#endif  
