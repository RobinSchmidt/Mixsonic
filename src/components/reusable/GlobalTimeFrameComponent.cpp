#include "GlobalTimeFrameComponent.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction:

GlobalTimeFrameComponent::GlobalTimeFrameComponent(const String &componentName)
: Component(componentName)
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
  repaint();
}

//-------------------------------------------------------------------------------------------------
// inquiry:

int GlobalTimeFrameComponent::getDesiredClipWidth(Clip *theClip) const
{
  // calculate the desired width of the clip in pixels:
  double ratio = theClip->getActiveRegionDuration(SECONDS) / getLengthInSeconds();
  return jlimit(MIN_CLIPWIDTH, getWidth(), (int) (ratio*getWidth()) );
}

//-------------------------------------------------------------------------------------------------
// conversion functions:

double GlobalTimeFrameComponent::beatsToPixelPosition(double timeInBeats) const
{
  return mapLinearToLinear(timeInBeats, minTimeInBeats, maxTimeInBeats, 0.0, getWidth());
}

double GlobalTimeFrameComponent::pixelPositionToBeats(double pixelX) const
{
  return mapLinearToLinear(pixelX, 0.0, getWidth(), minTimeInBeats, maxTimeInBeats);
}

double GlobalTimeFrameComponent::secondsToPixelPosition(double timeInSeconds) const
{
  return mapLinearToLinear(timeInSeconds, getMinTimeInSeconds(), getMaxTimeInSeconds(), 
    0.0, getWidth());
}

double GlobalTimeFrameComponent::pixelPositionToSeconds(double pixelX) const
{
  return mapLinearToLinear(pixelX, 0.0, getWidth(), getMinTimeInSeconds(), getMaxTimeInSeconds());
}



