#ifndef GlobalTimeFrameComponent_h
#define GlobalTimeFrameComponent_h

#include "../../core/infrastructure/TimeFrame.h"
#include "../../core/infrastructure/Clip.h"

/**

This class is a component which serves as baseclass for components which have some underlying 
global time-frame such as the timeline-ruler or tracks in sequencer applications.

\todo maybe we should use composition instead of inheritance (i.e. have a TimeFrame member)
or: get rid of deriving from Component - instead pass a compüonent-width into the conversion functions
-> this would suggest to rename this class into something like GlobalTimeCoordinateConverter or 
something



*/

class GlobalTimeFrameComponent : /*public Component,*/ public TimeFrame
{  

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  GlobalTimeFrameComponent(const String &componentName = String("GlobalTimeFrameComponent"));  

  /** Destructor. */
  virtual ~GlobalTimeFrameComponent();                             

  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Overrides the method from TimeFrame in order to repaint itself. */
  virtual void setTimeRangeInSeconds(double newMinTime, double newMaxTime);

  /* \todo: override all the other setTime... methods as well */

  //-----------------------------------------------------------------------------------------------
  // inquiry:

  /** Returns the desired width of a clip in pixels according to the ratio of the clip's length 
  to the total length of the time-slice shown in the arrangementPanel. */
  virtual int getDesiredClipWidth(Clip *theClip, int pixelWidth) const;

  //-----------------------------------------------------------------------------------------------
  // conversion functions:

  /** Converts a time value given in the beats into an x-coordinate of a pixel such that the 
  minimum time maps to 0 and the maximum time maps to the width of the component. */
  virtual double beatsToPixelPosition(double timeInBeats, int pixelWidth) const;

  /** Converts a pixel's x-coordinate into a time value in beats. */
  virtual double pixelPositionToBeats(double pixelX, int pixelWidth) const;

  /** Converts a time value given in the seconds into an x-coordinate of a pixel such that the 
  minimum time maps to 0 and the maximum time maps to the width of the component. */
  virtual double secondsToPixelPosition(double timeInSeconds, int pixelWidth) const;

  /** Converts a pixel's x-coordinate into a time value in seconds. */
  virtual double pixelPositionToSeconds(double pixelX, int pixelWidth) const;

  //===============================================================================================
  juce_UseDebuggingNewOperator;

};

#endif  