#ifndef MixsonicSlider_h
#define MixsonicSlider_h

#include "RWidget.h"
#include "MixsonicSliderListener.h"

/**

This is a simplified slider class specifically tailored for the requirements of the Mixsonic 
application - inheriting form juce::Slider was not possible because of some of its behaviours 
which could not be overriden.

*/

class MixsonicSlider : public Component, public RWidget, protected AsyncUpdater
{

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  MixsonicSlider(const String& componentName = String::empty);

  /** Destructor. */
  virtual ~MixsonicSlider();

  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Sets up the range of the slider, a quantization interval for the values, a default value and
  optionally initializes the current value to the default value. */
  virtual void setRange(double newMinimum, double newMaximum, double newInterval, 
    double newDefaultValue, bool initToDefault = true);

  /** Sets the current value of the slider and optionally sends out a callback message. */
  virtual void setValue(double newValue, const bool sendUpdateMessage = true, 
    const bool sendMessageSynchronously = false);

  /** Switches the slider into vertical mode of operation - the default mode is horizontal. */
  virtual void setVertical(bool shouldBeVertical);

  /** Sets the fill colour for the slider's thumb. */
  virtual void setThumbColour(Colour newThumbColour);

  /** Sets the colour for the slider's position marker. */
  virtual void setPositionMarkerColour(Colour newPositionMarkerColour);

  /** Adds a listener to this slider whcih will be called back when the slider's value has 
  changed. */
  virtual void addListener (MixsonicSliderListener* const listener) throw();

  /** Removes a listener form this slider. */
  virtual void removeListener (MixsonicSliderListener* const listener) throw();

  //-----------------------------------------------------------------------------------------------
  // inquiry:

  /** Returns the current value of the slider. */
  virtual double getValue()    const { return currentValue; };

  /** Returns the maximum value of the slider. */
  virtual double getMaximum()  const { return maxValue; }

  /** Returns the minimum value of the slider. */
  virtual double getMinimum()  const { return minValue; }

  /** Returns the quantization interval for the values of the slider. */
  virtual double getInterval() const { return interval; }

  //-----------------------------------------------------------------------------------------------
  // callbacks:

  /** Overrides mouseEnter in order to call the method from the baseclass RWidget (as opposed to 
  Component). */
  virtual void mouseEnter(const MouseEvent &e);

  /** Overrides mouseExit in order to call the method from the baseclass RWidget (as opposed to 
  Component). */
  virtual void mouseExit(const MouseEvent &e);

  /** Overrides mouseDown to update the slider according to the mouse-position. */
  virtual void mouseDown (const MouseEvent& e);

  /** Overrides mouseDrag to update the slider according to the mouse-position. */
  virtual void mouseDrag (const MouseEvent& e);

  /** Overrides mouseWheelMove in order adjust the slider on such events */
  virtual void mouseWheelMove (const MouseEvent& e, float wheelIncrementX, float wheelIncrementY);

  /** Paints the slider. */
  virtual void paint(Graphics& g);

  //-----------------------------------------------------------------------------------------------
  // others:

  /** Converts a value representing a proportion of the sliders length (assumed to be in 0...1) to
  the corresponding value of the slider. */
  virtual double proportionOfLengthToValue(double proportion);

  /** Converts a value of the slider into a value that represents a proportion of the sliders 
  length (in the range 0...1). */
  virtual double valueToProportionOfLength(double value);


  //===============================================================================================
  juce_UseDebuggingNewOperator;

protected:

  /** Returns a value that is constrained to the range of the slider and quantized to the interval
  as set by setRange(). */
  virtual double constrainedValue(double value) const throw();

  /** Constrains the members currentValue and defaultValue via constrainedValue(). */
  void valueSanityCheck();

  // these functions were taken over from juce::Slider 'as is':
  virtual void handleAsyncUpdate();
  virtual void triggerChangeMessage(const bool synchronous);

  // our listeners:
  SortedSet<void*> listeners;

  // internal state variables:
  double currentValue, minValue, maxValue, defaultValue, interval;
  bool   isVertical;

  // colours for various aspects of the slider
  Colour backgroundColour, outlineColour, thumbColour, positionMarkerColour;
  
private:

  // make copy constructor and assignment operator unavailable:
  MixsonicSlider(const MixsonicSlider&);
  const MixsonicSlider& operator= (const MixsonicSlider&);

};

#endif   
