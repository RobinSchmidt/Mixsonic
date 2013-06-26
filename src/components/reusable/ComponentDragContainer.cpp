#include "ComponentDragContainer.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction:

ComponentDragContainer::ComponentDragContainer(const String &componentName) 
: GrabbableComponent(componentName)
{
  Component::setName(componentName); // is this necessary? probably not.
}

ComponentDragContainer::~ComponentDragContainer()
{
  deleteAllChildren();
}

//-------------------------------------------------------------------------------------------------
// callbacks:

void ComponentDragContainer::paintOverChildren(Graphics &g)
{
  g.setColour(Colours::green.withAlpha(0.5f));
  g.drawRect(0, 0, getWidth(), getHeight(), 2);
}



