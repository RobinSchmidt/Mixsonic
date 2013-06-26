#ifndef SelectionListener_h
#define SelectionListener_h

#include "../../libs/JuceIncludes.h"

// forward declaration of the SelectableComponent class:
class SelectableItem;

/**

This class is the base-class for all objects that are interested in listening to changes of the 
selection-state of some SelectableComponent.

*/

class SelectionListener
{  

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  SelectionListener();  

  /** Destructor. */
  virtual ~SelectionListener();                             

  //-----------------------------------------------------------------------------------------------
  // callbacks:

  /** Callback function that may be overriden by your subclass in order to respond to changes of 
  the selection state of some SelectableComponent. */
  virtual void selectionStateChanged(SelectableItem *itemThatHasChanged);

  /** Callback function that may be overriden by your subclass in order to respond to changes of 
  the selection state of some SelectableComponent that results from a MouseEvent. */
  virtual void selectionStateChangedByMouse(SelectableItem *itemThatHasChanged, 
    const MouseEvent &e);

  //===============================================================================================
  juce_UseDebuggingNewOperator;

};

#endif  