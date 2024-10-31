#ifndef TimeLineComponent_h
#define TimeLineComponent_h

#include "GlobalTimeFrameComponent.h"
//#include "../../control/Skin.h"
//#include "../../components/reusable/UserInterfaceSection.h"
#include "../../components/widgets/MWidget.h"


/**

This class is a component which serves as timeline-ruler for sequencer applications.

*/

class TimeLineComponent : public Component, public GlobalTimeFrameComponent, public ChangeBroadcaster,
  public MWidget
  //public UserInterfaceSection // maybe derive from MWidget instead
{  

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  TimeLineComponent(Skin *skinToUse, const String &componentName = String("TimeLineComponent"));  
  //TimeLineComponent(const String &componentName = String("TimeLineComponent"));  

  /** Destructor. */
  virtual ~TimeLineComponent();                             

  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Overrides the method from TimeUnitConverter in order to repaint. */
  virtual void setSelectedTimeUnit(int newSelectedTimeUnit);

  //-----------------------------------------------------------------------------------------------
  // callbacks:

  /** Responds to mouseDown events by updating the currentTimeInSeconds and sending out a 
  change-message (to be received by the ArrangementPanel). */
  virtual void mouseDown(const MouseEvent &e);

  /** Paints the timeline. */
  virtual void paint(Graphics &g);

  //===============================================================================================
  juce_UseDebuggingNewOperator;

protected:

  /** Returns the string to appear to the left of a coarse division line along the ruler. */
  virtual String getDivisionStringLeft(double timeInCurrentUnit) const;

  /** Returns the string to appear to the right of a coarse division line along the ruler. */
  virtual String getDivisionStringRight(double timeInCurrentUnit) const;

};

#endif  