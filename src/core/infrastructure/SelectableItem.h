#ifndef SelectableItem_h
#define SelectableItem_h

#include "SelectionListener.h"

/**

This class represents an item that can be selected or deselected via mouseclicks inside it and will 
call the callback function selectionStateChanged for all its attached SelectionListener 
objects whenever this selection state changes (unless the callback is supressed by passing 'false' 
as second argument to setSelected). The current selection state can be requested via isSelected and 
set from outside via setSelected(). The class also provides a means to temporarily select items - 
this is useful when selecting a bunch of item via some kind of lasso selector: when the items that 
are currently under the lasso should be drawn as if they were selected, but are not yet really 
selected (which happens only when the user has finished the lasso action). To support such things, 
a similar functionality as for the actual selection is provided by setTemporarilySelected, 
setTemporarilySelectedByMouse and isTemporarilySelected.

*/

class SelectableItem 
{  

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  SelectableItem();  

  /** Destructor. */
  virtual ~SelectableItem();                             

  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Adds a ItemSelectionListener to this SelectableItem - this listener will be called 
  when the selectionState is changed via setSelected(). */
  virtual void addSelectionListener(SelectionListener* listenerToAdd);

  /** Removes a ItemSelectionListener from this SelectableItem. */
  virtual void removeSelectionListener(SelectionListener* listenerToRemove);

  /** Sets the selection state, clears the temporarilySelected flag and optionally sends a message 
  to our listeners (by calling their selectionStateChange callback functions). */
  virtual void setSelected(bool shouldBeSelected, bool sendSelectionStateChangeMessage = true);

  /** This will be called when the selection state is changed by a MouseEvent.
  @see: setSelected(bool shouldBeSelected, bool sendSelectionStateChangeMessage = true) */
  virtual void setSelectedByMouse(bool shouldBeSelected, const MouseEvent& e,
    bool sendSelectionStateChangeMessage = true);

  /** Sets the temporary selection state and optionally sends a message to our listeners (by 
  calling their selectionStateChange callback functions). */
  virtual void setTemporarilySelected(bool shouldBeTemporarilySelected, 
    bool sendSelectionStateChangeMessage = true);

  /** This will be called when the temporary selection state is changed by a MouseEvent.
  @see: setTemprarilySelected(bool shouldBeTemporarilySelected, 
  bool sendSelectionStateChangeMessage = true) */
  virtual void setTemporarilySelectedByMouse(bool shouldBeTemporarilySelected, const MouseEvent& e,
    bool sendSelectionStateChangeMessage = true);

  //-----------------------------------------------------------------------------------------------
  // inquiry:

  /** Informs whether or not this component is currently in selected state. */
  virtual bool isSelected() const { return selected; }

  /** Informs whether or not this component is currently in temporarily selected state. */
  virtual bool isTemporarilySelected() const { return temporarilySelected; }

  //-----------------------------------------------------------------------------------------------
  // callbacks:

  /** Overrides mouseDown in order to change the selection state on mouseclicks. */
  virtual void mouseDown(const MouseEvent &e);

  //===============================================================================================
  juce_UseDebuggingNewOperator;

protected:

  Array<SelectionListener*> listeners;
  bool selected, temporarilySelected;

};

#endif  