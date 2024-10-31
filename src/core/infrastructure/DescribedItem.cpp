#include "DescribedItem.h"

DescribedItem::DescribedItem(const String& newDescription) 
{
  description       = newDescription;
  descriptionField  = NULL;
}

void DescribedItem::setDescription(const String &newDescription)
{
  description = newDescription;
}

void DescribedItem::setDescriptionField(Label *newDescriptionField)
{
  descriptionField = newDescriptionField;
}

String DescribedItem::getDescription() const
{
  return description;
}

Label* DescribedItem::getDescriptionField() const
{
  return descriptionField;
}

//=================================================================================================

DescribedMouseListener::DescribedMouseListener(const String& newDescription) 
: DescribedItem(newDescription)
{

}

void DescribedMouseListener::mouseEnter(const juce::MouseEvent &e)
{
  if( descriptionField != NULL )
    descriptionField->setText(description, false);
}

void DescribedMouseListener::mouseExit(const MouseEvent &e)
{
  if( descriptionField != NULL )
    descriptionField->setText(String::empty, false);
}

//=================================================================================================

DescribedComponent::DescribedComponent(const String& newDescription) 
: DescribedItem(newDescription)
{

}

void DescribedComponent::mouseEnter(const juce::MouseEvent &e)
{
  if( descriptionField != NULL )
    descriptionField->setText(description, false);
}

void DescribedComponent::mouseExit(const MouseEvent &e)
{
  if( descriptionField != NULL )
    descriptionField->setText(String::empty, false);
}

