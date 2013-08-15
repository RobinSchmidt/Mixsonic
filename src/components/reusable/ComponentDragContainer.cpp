#include "ComponentDragContainer.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction:

ComponentDragContainer::ComponentDragContainer(const String &componentName) 
: GrabbableComponent(componentName)
{
  Component::setName(componentName); // is this necessary? probably not.
  outlineColor = Colours::red.withAlpha(0.5f); // preliminary
}

ComponentDragContainer::~ComponentDragContainer()
{
  deleteAllChildren();
}

//-------------------------------------------------------------------------------------------------
// setup:

void ComponentDragContainer::setOutlineColor(const Colour &newOutlineColor)
{
  outlineColor = newOutlineColor;
}

//-------------------------------------------------------------------------------------------------
// callbacks:

void ComponentDragContainer::paintOverChildren(Graphics &g)
{
  g.setColour(outlineColor);
  g.drawRect(0, 0, getWidth(), getHeight(), 2);
}



