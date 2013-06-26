#ifndef ThreadedDrawingPanel_h
#define ThreadedDrawingPanel_h

#include "ThreadedDrawingComponent.h"
#include "Panel.h"


/**

This class combines the functionality of Panel with the feature of redrawing itself in a separate 
thread inherited from ThreadedDrawingPanel

*/

class ThreadedDrawingPanel: virtual public Panel, virtual public ThreadedDrawingComponent
{

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  ThreadedDrawingPanel(TimeSliceThread* newThreadToUse = NULL);   

  //-----------------------------------------------------------------------------------------------
  // overrides:
  virtual void resized();
  virtual void setDirty(bool shouldBeSetToDirty = true);

  //===============================================================================================
  juce_UseDebuggingNewOperator;

private:

  virtual void paint(Graphics &g);

};

#endif  
