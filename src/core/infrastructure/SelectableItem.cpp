#include "SelectableItem.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction:

SelectableItem::SelectableItem() 
{
  selected            = false;
  temporarilySelected = false;
}

SelectableItem::~SelectableItem()
{

}

//-------------------------------------------------------------------------------------------------
// setup:

void SelectableItem::addSelectionListener(SelectionListener* listenerToAdd)
{
  listeners.getLock().enter();
  listeners.addIfNotAlreadyThere(listenerToAdd);  
  listeners.getLock().exit();
}

void SelectableItem::removeSelectionListener(SelectionListener *listenerToRemove)
{
  listeners.getLock().enter();
  listeners.removeValue(listenerToRemove);
  listeners.getLock().exit();
}

void SelectableItem::setSelected(bool shouldBeSelected, bool sendSelectionStateChangeMessage)
{
  temporarilySelected = false;
  selected = shouldBeSelected;
  if( sendSelectionStateChangeMessage == true )
  {
    listeners.getLock().enter();
    for(int i=0; i<listeners.size(); i++)
      listeners[i]->selectionStateChanged(this);
    listeners.getLock().exit();
  }
}

void SelectableItem::setSelectedByMouse(bool shouldBeSelected, const MouseEvent &e, 
                                        bool sendSelectionStateChangeMessage)
{
  temporarilySelected = false;
  selected = shouldBeSelected;
  if( sendSelectionStateChangeMessage == true )
  {
    listeners.getLock().enter();
    for(int i=0; i<listeners.size(); i++)
      listeners[i]->selectionStateChangedByMouse(this, e);
    listeners.getLock().exit();
  }
}

void SelectableItem::setTemporarilySelected(bool shouldBeTemporarilySelected, 
                                            bool sendSelectionStateChangeMessage)
{
  temporarilySelected = shouldBeTemporarilySelected;;
  if( sendSelectionStateChangeMessage == true )
  {
    listeners.getLock().enter();
    for(int i=0; i<listeners.size(); i++)
      listeners[i]->selectionStateChanged(this);
    listeners.getLock().exit();
  }
}

void SelectableItem::setTemporarilySelectedByMouse(bool shouldBeTemporarilySelected, 
                                                   const MouseEvent &e, 
                                                   bool sendSelectionStateChangeMessage)
{
  temporarilySelected = shouldBeTemporarilySelected;
  if( sendSelectionStateChangeMessage == true )
  {
    listeners.getLock().enter();
    for(int i=0; i<listeners.size(); i++)
      listeners[i]->selectionStateChangedByMouse(this, e);
    listeners.getLock().exit();
  }
}

//-------------------------------------------------------------------------------------------------
// callbacks:

void SelectableItem::mouseDown(const MouseEvent &e)
{
  setSelectedByMouse(!isSelected(), e, true);
}

