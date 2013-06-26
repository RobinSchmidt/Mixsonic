#include "RWidget.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction:

RWidget::RWidget(const String& newDescription) 
{
  // init the function poinetr for the string conversion:
  stringConversionFunction = &valueToString2;
}

RWidget::~RWidget()
{

}

//-------------------------------------------------------------------------------------------------
// callbacks:

void RWidget::mouseEnter(const juce::MouseEvent &e)
{
  if( descriptionField != NULL )
    descriptionField->setText(description, false);
}

void RWidget::mouseExit(const MouseEvent &e)
{
  if( descriptionField != NULL )
    descriptionField->setText(String::empty, false);
}
