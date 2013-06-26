#include "SelectionListener.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction:

SelectionListener::SelectionListener() 
{

}

SelectionListener::~SelectionListener()
{

}

//-------------------------------------------------------------------------------------------------
// callbacks:

void SelectionListener::selectionStateChanged(SelectableItem *itemThatHasChanged)
{
  // you may want to do something here in your subclass
}

void SelectionListener::selectionStateChangedByMouse(SelectableItem *itemThatHasChanged, 
                                                     const juce::MouseEvent &e)
{
  // you may want to do something here in your subclass
}


