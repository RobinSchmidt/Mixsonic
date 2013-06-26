#include "DescribedItem.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction:

DescribedItem::DescribedItem(const String& newDescription) 
{
  description       = newDescription;
  descriptionField  = NULL;
}

DescribedItem::~DescribedItem()
{

}

//-------------------------------------------------------------------------------------------------
// setup:

void DescribedItem::setDescription(const String &newDescription)
{
  description = newDescription;
}

void DescribedItem::setDescriptionField(Label *newDescriptionField)
{
  descriptionField = newDescriptionField;
}

//-------------------------------------------------------------------------------------------------
// inquiry:

String DescribedItem::getDescription() const
{
  return description;
}

Label* DescribedItem::getDescriptionField() const
{
  return descriptionField;
}