#ifndef ComponentDragContainer_h
#define ComponentDragContainer_h

#include "GrabbableComponent.h"

/**

This class is component which facilitates grabbing and dragging multiple components at once.

*/

class ComponentDragContainer : public GrabbableComponent
{  

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  ComponentDragContainer(const String &componentName = String("ComponentDragContainer"));

  /** Destructor. */
  virtual ~ComponentDragContainer();

  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Sets the color for the outline of the rectangle. */
  virtual void setOutlineColor(const Colour &newOutlineColor);

  //-----------------------------------------------------------------------------------------------
  // callbacks:

  /** Overrides paints in order to draw an outlining reactangle. */
  virtual void paintOverChildren(Graphics &g);

  //===============================================================================================
  juce_UseDebuggingNewOperator;

protected:

  Colour outlineColor;

};

#endif  