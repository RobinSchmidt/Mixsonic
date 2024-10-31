#include "MixsonicSlider.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction:

MixsonicSlider::MixsonicSlider(Skin *skinToUse, const String& name) 
: Component(name)
, MWidget(skinToUse)
{
  currentValue         = 0.5;
  defaultValue         = 0.5;
  minValue             = 0.0;
  maxValue             = 1.0;
  interval             = 0.01;
  isVertical           = false;

  // get rid of these members and use the colors from the Skin directly in the paint method:
  backgroundColour     = skin->backgroundColor;
  outlineColour        = skin->outlineColor;
  thumbColour          = skin->middlegroundColor;
  positionMarkerColour = skin->foregroundColor;
  /*
  backgroundColour     = Skin::getInstance()->widgetBackgroundColor;
  outlineColour        = Skin::getInstance()->outlineColor;
  thumbColour          = Skin::getInstance()->widgetHandleColor;
  positionMarkerColour = Skin::getInstance()->markerColor;
  */
}

MixsonicSlider::~MixsonicSlider()
{

}

//-------------------------------------------------------------------------------------------------
// setup:

void MixsonicSlider::setRange(double newMin, double newMax, double newInt, double newDef, 
                              bool initToDefault)
{
  jassert(newMin <  newMax);
  jassert(newInt >= 0.0);

  minValue     = newMin;
  maxValue     = newMax;
  interval     = newInt;
  defaultValue = newDef;

  valueSanityCheck();

  // initialize to default-value or keep the current value inside the new range:
  if( initToDefault == true )
    setValue(defaultValue, false, false);
  else
    setValue(currentValue, false, false);

  repaint();
}

void MixsonicSlider::setValue(double newValue, const bool sendUpdateMessage, 
                              const bool sendMessageSynchronously)
{
  newValue = constrainedValue(newValue);
  valueSanityCheck();

  if( currentValue != newValue )
  {
    currentValue = newValue;
    repaint();
    if(sendUpdateMessage)
      triggerChangeMessage(sendMessageSynchronously);
  }
}

void MixsonicSlider::setVertical(bool shouldBeVertical)
{
  isVertical = shouldBeVertical;
}

void MixsonicSlider::setThumbColour(Colour newThumbColour)
{
  thumbColour = newThumbColour;
}

void MixsonicSlider::setPositionMarkerColour(Colour newPositionMarkerColour)
{
  positionMarkerColour = newPositionMarkerColour;
}

void MixsonicSlider::addListener(MixsonicSliderListener* const listener) throw()
{
  jassert( listener != 0 );
  if( listener != 0 )
    listeners.add(listener);
}

void MixsonicSlider::removeListener(MixsonicSliderListener* const listener) throw()
{
  listeners.removeValue(listener);
}

//-------------------------------------------------------------------------------------------------
// callbacks:
/*
void MixsonicSlider::mouseEnter(const MouseEvent &e)
{
  MWidget::mouseEnter(e);
}

void MixsonicSlider::mouseExit(const MouseEvent &e)
{
  MWidget::mouseExit(e);
}
*/
void MixsonicSlider::mouseDown(const MouseEvent& e)
{
  if( isEnabled() )
  {
    if( e.mods.isCommandDown() )
      setValue(defaultValue);
    else if( e.mods.isLeftButtonDown() )
    {
      if( isVertical )
        setValue(proportionOfLengthToValue(1.0 - (double) e.y/(double) getHeight()), true, false);
      else
        setValue(proportionOfLengthToValue((double) e.x / (double) getWidth()), true, false);
    }
  }
}

void MixsonicSlider::mouseDrag(const MouseEvent& e)
{
  if( isEnabled() == false )
    return;

  if( !e.mods.isRightButtonDown() && !e.mods.isCommandDown() )
  {
    double excursion, totalLength;
    if( isVertical )
    {
      excursion   = e.getMouseDownY() + e.getDistanceFromDragStartY();
      totalLength = getHeight();
      setValue(proportionOfLengthToValue(1.0-excursion/totalLength), true, false);
    }
    else
    {
      excursion   = e.getMouseDownX() + e.getDistanceFromDragStartX();
      totalLength = getWidth();
      setValue(proportionOfLengthToValue(excursion/totalLength), true, false);
    }
  }
}

void MixsonicSlider::mouseWheelMove(const MouseEvent& e, float wheelIncrementX, 
                                    float wheelIncrementY)
{
  if( isEnabled() )
  {
    float s;
    if( wheelIncrementY >= 0.0 )
      s = 1.0;
    else
      s = -1.0;

    if( interval > 0.0 )
      setValue(getValue() + s * interval, true, false) ;
    else
      setValue(getValue() + 0.01*wheelIncrementY*(maxValue-minValue), true, false);
  }
}

void MixsonicSlider::paint(Graphics& g)
{
  int w = getWidth();
  int h = getHeight();

  if( w < 1 || h < 1 )
    return;

  g.fillAll(backgroundColour);

  g.setColour(thumbColour);
  if( isVertical )
  {
    double thumbHeight = h * valueToProportionOfLength(getValue());

    // new:
    ColourGradient gradient(backgroundColour, 0.f, (float) getHeight(), Colours::white, 0.f, 0.f, 
      false);
    g.setGradientFill(gradient);

    g.fillRect(0.f, (float) (h-thumbHeight), (float) w, (float) thumbHeight);
    g.setColour(positionMarkerColour);
    g.drawLine(0.f, (float) (h-thumbHeight), (float) w, (float) (h-thumbHeight), 2.f);
  }
  else
  {
    //double thumbWidth = w * valueToProportionOfLength(getValue());

    double p = valueToProportionOfLength(getValue());
    double thumbStart, thumbWidth;
    if( p <= 0.5 )
    {
      thumbStart = w * p;
      thumbWidth = w * (0.5 - p);
    }
    else
    {
      thumbStart = 0.5 * w;
      thumbWidth = w * (p - 0.5);
    }

    ColourGradient gradient(Colours::white, 0.f, 0.f, Colours::white, (float) getWidth(), 0.f, false);
    gradient.addColour(0.5, Colour::greyLevel(0.5f));
    g.setGradientFill(gradient);
    g.fillRect((float)thumbStart, 0.f, (float)thumbWidth, (float)h);
    

    //g.setColour(positionMarkerColour);
    g.setColour(Colour::greyLevel((float)(0.5+fabs(0.5-p)))); // preliminary
    g.drawLine((float)(w*p), 0.f, (float)(w*p), (float) h, 2.f);
  }
}

//-------------------------------------------------------------------------------------------------
// others:

double MixsonicSlider::proportionOfLengthToValue(double proportion)
{
  return minValue + (maxValue - minValue) * proportion;
}

double MixsonicSlider::valueToProportionOfLength(double value)
{
  if( maxValue > minValue ) // ...to avoid division by zero errors
    return (value - minValue) / (maxValue - minValue);
  else
    return 0.0;
}

//-------------------------------------------------------------------------------------------------
// internal functions:

double MixsonicSlider::constrainedValue(double value) const throw()
{
  if( interval > 0 )
    value = minValue + interval * floor ((value - minValue) / interval + 0.5);

  if( value <= minValue || maxValue <= minValue )
    value = minValue;
  else if( value >= maxValue )
    value = maxValue;

  return value;
}

void MixsonicSlider::valueSanityCheck()
{
  jassert( maxValue > minValue );
  if( maxValue <= minValue )
    maxValue = minValue+1.0;
  currentValue = constrainedValue(currentValue);
  defaultValue = constrainedValue(defaultValue);
}

void MixsonicSlider::handleAsyncUpdate()
{
  cancelPendingUpdate();
  for(int i = listeners.size(); --i >= 0;)
  {
    ((MixsonicSliderListener*) listeners.getUnchecked (i))->mSliderValueChanged(this);
    i = jmin(i, listeners.size());
  }
}

void MixsonicSlider::triggerChangeMessage (const bool synchronous)
{
  if(synchronous)
    handleAsyncUpdate();
  else
    triggerAsyncUpdate();
}
