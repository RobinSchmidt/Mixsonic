#include "RPopUpComponent.h"

//-----------------------------------------------------------------------------------------------------------------------------------------
// construction/destruction:
   
RPopUpComponent::RPopUpComponent(const Skin *skinToUse) : RWidget(skinToUse)
{
  //componentAttachedTo = NULL;
  setWantsKeyboardFocus(true);
  contentComponent    = NULL;  
  open                = false;
  dismissOnFocusLoss  = true;
}

RPopUpComponent::~RPopUpComponent()
{
  dismiss();
  deleteAllChildren();
}

//-----------------------------------------------------------------------------------------------------------------------------------------
// setup:

void RPopUpComponent::setContentComponent(Component *newContentComponent)
{
  dismiss();
  deleteAllChildren();
  contentComponent = newContentComponent;
  addAndMakeVisible(contentComponent);
}

void RPopUpComponent::setContentWidget(RWidget *newContentComponent)
{
  setContentComponent( newContentComponent );
  //addChildWidget(newContentComponent, false, false);
}

//-----------------------------------------------------------------------------------------------------------------------------------------
// callbacks:

void RPopUpComponent::inputAttemptWhenModal()
{
  dismiss();
  // \todo maybe define a user selectable behavior of what to do when this happens...
}

void RPopUpComponent::focusLost(FocusChangeType cause)
{
  if( dismissOnFocusLoss == true )
    dismiss();
}

bool RPopUpComponent::canModalEventBeSentToComponent(const Component* targetComponent)
{
  if( targetComponent == contentComponent )
    return true;
  else
    return false;
}

/*
void RPopUpComponent::mouseEnter(const MouseEvent &e)
{

}

void RPopUpComponent::mouseExit(const MouseEvent &e)
{

}
*/

void RPopUpComponent::mouseDown(const MouseEvent &e)
{

  int dummy = 0;
}

/*
void RPopUpComponent::mouseMove(const MouseEvent &e)
{  

}

void RPopUpComponent::paint(Graphics &g)
{

}
*/

void RPopUpComponent::showAtNonModal(int screenX, int screenY, int width, int height)
{
  jassert( contentComponent != NULL ); // you must pass a content-component before trying to show the popup

  if( width < 1 )
    width = contentComponent->getWidth();
  if( height < 1 )
    height = contentComponent->getHeight();

  setBounds(screenX, screenY, width, height);
  //contentComponent->setSize(width, height);
  Desktop::getInstance().addGlobalMouseListener(this);
  addToDesktop(ComponentPeer::windowHasDropShadow | ComponentPeer::windowIsTemporary | ComponentPeer::windowIsSemiTransparent);
  //addToDesktop(ComponentPeer::windowHasDropShadow | ComponentPeer::windowIsSemiTransparent);



  open = true;
  setVisible(true);
  toFront(true);
}

void RPopUpComponent::showAt(bool showModally, int screenX, int screenY, int width, int height)
{
  showAtNonModal(screenX, screenY, width, height);
  if( showModally == true )
    runModalLoop();
}

void RPopUpComponent::showAtMousePosition(bool showModally, int width, int height)
{
  Point<int> mousePosition = Desktop::getMousePosition();
  showAt(showModally, mousePosition.getX(), mousePosition.getY(), width, height);
}

//-----------------------------------------------------------------------------------------------------------------------------------------
// others:

void RPopUpComponent::dismiss()
{
  //componentAttachedTo = NULL;
  open = false;
  setVisible(false);

  Desktop::getInstance().removeGlobalMouseListener(this);
  // removeFromDesktop(); // this seems to mess with mouse-events that should reach the GUI after the popup was closed when the last click
                          // occured on a part of the popup that was outside th GUI
  exitModalState(0);
}


//=========================================================================================================================================
// class ROwnedPopUpComponent:


ROwnedPopUpComponent::ROwnedPopUpComponent(Component *ownerComponent, const Skin *skinToUse) 
: ComponentMovementWatcher(ownerComponent)
, RPopUpComponent(skinToUse)
{
  this->ownerComponent = ownerComponent;
}

    
void ROwnedPopUpComponent::focusLost(FocusChangeType cause)
{
  if( dismissOnFocusLoss == true )
  {
    RPopUpOwner *po = dynamic_cast<RPopUpOwner*> (ownerComponent);
    if( po != NULL && isMouseDownOnOwner() )
      po->rPopUpDismissedByClickOnOwner(this);
    dismiss();
  }
}

void ROwnedPopUpComponent::componentMovedOrResized(bool wasMoved, bool wasResized)
{

}
 
void ROwnedPopUpComponent::componentPeerChanged()
{

}

void ROwnedPopUpComponent::componentVisibilityChanged()
{

}

bool ROwnedPopUpComponent::canModalEventBeSentToComponent(const Component* targetComponent)
{
  if( targetComponent == ownerComponent )
    return true;
  else
    return RPopUpComponent::canModalEventBeSentToComponent(targetComponent);
}

void ROwnedPopUpComponent::show(bool showModally, int attachPosition, int width, int height, int xOffset, int yOffset)
{

  Rectangle<int> ownerBounds = ownerComponent->getScreenBounds();
  switch( attachPosition )
  {
  case BELOW:
    showAt(showModally, ownerBounds.getX()+xOffset, ownerBounds.getY()+ownerBounds.getHeight()+yOffset, width, height);
    break;

  
    // \todo: implement the other attachPositions
  }
}

bool ROwnedPopUpComponent::isMouseDownOnOwner()
{
  Rectangle<int> ownerBounds = ownerComponent->getScreenBounds();
  Point<int>     mousePos    = Desktop::getMousePosition();
  if( ownerBounds.contains(mousePos) )
    return true;
  else
    return false;
}
