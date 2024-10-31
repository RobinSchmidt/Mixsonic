#include "GlobalTimeFrameComponent.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction:

GlobalTimeFrameComponent::GlobalTimeFrameComponent(const String &componentName)
/*: Component(componentName)*/
{

}

GlobalTimeFrameComponent::~GlobalTimeFrameComponent()
{

}

//-------------------------------------------------------------------------------------------------
// setup:

void GlobalTimeFrameComponent::setTimeRangeInSeconds(double newMinTime, double newMaxTime)
{
  TimeFrame::setTimeRangeInSeconds(newMinTime, newMaxTime);
  //repaint();
}

//-------------------------------------------------------------------------------------------------
// inquiry:

int GlobalTimeFrameComponent::getDesiredClipWidth(Clip *theClip, int pixelWidth) const
{
  // calculate the desired width of the clip in pixels:
  double ratio = theClip->getActiveRegionDuration(SECONDS) / getLengthInSeconds();
  return jlimit(MIN_CLIPWIDTH, pixelWidth, (int) (ratio*pixelWidth) );
}

//-------------------------------------------------------------------------------------------------
// conversion functions:

double GlobalTimeFrameComponent::beatsToPixelPosition(double timeInBeats, int pixelWidth) const
{
  return mapLinearToLinear(timeInBeats, minTimeInBeats, maxTimeInBeats, 0.0, pixelWidth);
}

double GlobalTimeFrameComponent::pixelPositionToBeats(double pixelX, int pixelWidth) const
{
  return mapLinearToLinear(pixelX, 0.0, pixelWidth, minTimeInBeats, maxTimeInBeats);
}

double GlobalTimeFrameComponent::secondsToPixelPosition(double timeInSeconds, int pixelWidth) const
{
  return mapLinearToLinear(timeInSeconds, getMinTimeInSeconds(), getMaxTimeInSeconds(), 
    0.0, pixelWidth);
}

double GlobalTimeFrameComponent::pixelPositionToSeconds(double pixelX, int pixelWidth) const
{
  return mapLinearToLinear(pixelX, 0.0, pixelWidth, getMinTimeInSeconds(), getMaxTimeInSeconds());
}



