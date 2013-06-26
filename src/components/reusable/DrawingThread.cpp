#include "DrawingThread.h"

//-------------------------------------------------------------------------------------------------
// construction and definition of the global instance:

DrawingThread drawingThread;

DrawingThread::DrawingThread() : TimeSliceThread(String(T("DrawingThread")))
{
  startThread(1);
}








