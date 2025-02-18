#ifndef DrawingThread_h
#define DrawingThread_h

#include "../../core/infrastructure/GlobalFunctions.h"

/**

This class implements a thread to be used for the drawing operation of objects of class 
Panel (or subclasses there of). It derives from TimeSliceThread and overrides the constructor in 
order to start itself inside this constructor. Panels register themselves as TimeSliceClient to 
the global instance 'drawingThread' - this already taken care of in the constructor of class 
Panel (and also the de-registering in the destructor). 

*/

class DrawingThread	: public TimeSliceThread
{

public:

  /** Constructor. */
  DrawingThread(); 

  //===============================================================================================
  juce_UseDebuggingNewOperator;

};

extern DrawingThread drawingThread; // a global instance of this class

#endif  
