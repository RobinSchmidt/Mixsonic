#include "TimeLineComponent.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction:

TimeLineComponent::TimeLineComponent(Skin *skinToUse, const String &componentName) 
: GlobalTimeFrameComponent(componentName)
, RWidget(skinToUse)
{
  Component::setName(componentName);
}

TimeLineComponent::~TimeLineComponent()
{

}

//-------------------------------------------------------------------------------------------------
// setup:

void TimeLineComponent::setSelectedTimeUnit(int newSelectedTimeUnit)
{
  GlobalTimeFrameComponent::setSelectedTimeUnit(newSelectedTimeUnit);
  repaint();
}

//-------------------------------------------------------------------------------------------------
// callbacks:

void TimeLineComponent::mouseDown(const MouseEvent &e)
{
  currentTimeInBeats = pixelPositionToBeats(e.x);
  sendChangeMessage();
}

void TimeLineComponent::paint(Graphics &g)
{
  g.fillAll(skin->backgroundColor);
  g.setColour(skin->outlineColor);
  g.drawLine(0.f, (float) getHeight(), (float) getWidth(), (float) getHeight(), 1.f);
  g.setColour(skin->foregroundColor);

  double tMin, tMax, tRange, tStep;
  int    i = 0;
  int    tBiggerStepFactor, tBiggestStepFactor;
  float  h = (float) getHeight();
  float  x, top;
  switch( selectedTimeUnit )
  {
  case BEATS: 
    {
      tMin               = getMinTimeInBeats();
      tMax               = getMaxTimeInBeats();  
      tStep              = 1;
      tBiggerStepFactor  = 4; 
      tBiggestStepFactor = 32;
    }
    break;
  case SECONDS: 
    {
      tMin               = getMinTimeInSeconds();
      tMax               = getMaxTimeInSeconds();  
      tStep              = 1.0; 
      tBiggerStepFactor  = 5;
      tBiggestStepFactor = 10;
    }
    break;
    /*
  case SAMPLES: 
    {
      tStep              = systemSampleRate;
      tBiggerStepFactor  = 5;
      tBiggestStepFactor = 10;
    }
    break;
    */

  }    
  tRange = tMax-tMin;

  // \todo: choose a proper time step based on visible range and take time-signature into 
  // account for a beat-based timeline (currently 4/4 is assumed)

  // draw the small, bigger and biggest subdivision lines and draw numbers next to the biggest 
  // lines:
  String s;

  //double t = floor(tMin);
  g.setFont(skin->normalFont);
  double t = floor(tMin - fmod(tMin, tBiggestStepFactor) );  // start at some large time division
  while( t <= tMax )
  {
    if( selectedTimeUnit == BEATS )
      x = (float) beatsToPixelPosition(t);
    else
      x = (float) secondsToPixelPosition(t);


    if( (i%tBiggestStepFactor) == 0 )
    {
      s = getDivisionStringLeft(t);
      g.drawText(s, (int) x-34, 0, 32, (int) h, Justification::topRight, false);

      s = getDivisionStringRight(t);
      g.drawText(s, (int) x+2, 0, 32, (int) h, Justification::topLeft, false);
      top = 0.f;
    }
    else if( (i%tBiggerStepFactor) == 0 )
      top = 0.5f*h;
    else
      top = 0.75f*h;

    g.drawLine(x, top, x, h, 1.f);

    i++;
    t += tStep;
  }
}

//-------------------------------------------------------------------------------------------------
// internal functions:

String TimeLineComponent::getDivisionStringLeft(double timeInCurrentUnit) const
{
  switch( selectedTimeUnit )
  {
  case BEATS:
    {
      int intTime     = (int) round(timeInCurrentUnit);
      int intQuotient = intTime / 4;  
      // preliminary:
      if( intQuotient < 1 )
        return String::empty;
      else
        return String(intQuotient) + String(":"); 

    };

    // \todo: treat the SAMPLES case

  default:
    {
      int intTime     = (int) round(timeInCurrentUnit);
      int intQuotient = intTime / 60;
      if( intQuotient < 1 )
        return String::empty;
      else
        return String(intQuotient) + String(":");
    }
  }
}

String TimeLineComponent::getDivisionStringRight(double timeInCurrentUnit) const
{

  switch( selectedTimeUnit )
  {
  case BEATS:
    {
      int intTime   = (int) round(timeInCurrentUnit);
      int remainder = intTime % 4;
        return String(remainder);
    };

    // \todo: treat the SAMPLES case

  default:
    {
      int intTime   = (int) round(timeInCurrentUnit);
      int remainder = intTime % 60;
      if( remainder >= 10 )
        return String(remainder);
      else
        return String(0) + String(remainder);
    }
  }
}