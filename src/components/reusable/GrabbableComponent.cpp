#include "GrabbableComponent.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction:

GrabbableComponent::GrabbableComponent(const String &componentName) : Component(componentName)
{
  grabber = NULL;
}

GrabbableComponent::~GrabbableComponent()
{

}

//-------------------------------------------------------------------------------------------------
// setup:

void GrabbableComponent::setComponentGrabber(ComponentGrabber* newGrabber)
{
  grabber = newGrabber;
}

//-------------------------------------------------------------------------------------------------
// callbacks:

void GrabbableComponent::mouseDown(const MouseEvent &e)
{
  if( grabber != NULL )
    grabber->componentRequestsToBeGrabbed(this, e);
}

