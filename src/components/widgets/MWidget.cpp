#include "MWidget.h"

MWidget::MWidget(Skin *skinToUse, const String& newDescription) 
: DescribedMouseListener(newDescription)
{
  stringConversionFunction = &valueToString2;  // function pointer for string conversion
  skin = skinToUse;
  jassert(skin != nullptr); // you should pass a valid pointer to a Skin object
}
