#ifndef RSlider_h
#define RSlider_h

#include "RPopUpMenu.h"
#include "../../control/Parameter.h"

class RSlider;
class RSliderListener
{
public:
  virtual void rSliderValueChanged(RSlider* rSlider) = 0;
};

/**

This is a class for horizontal sliders....

\todo: if needed, make a subclass VerticalSlider that overrides paint and the mouse-callbacks

*/

class RSlider : public RWidget, public RPopUpMenuObserver
{

public:

  /** An enumeration of the different layouts - this determines where the name of the slider 
  appears. */
  enum layouts
  {
    NAME_ABOVE = 0,
    NAME_LEFT,
    NAME_INSIDE
  };

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  RSlider(const Skin *skinToUse);

  /** Destructor. */
  virtual ~RSlider();

  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Sets up the name of this slider. This name will appear on the GUI. */
  virtual void setSliderName(const juce::String &newName);

  /** Chooses a layout. @see layouts */
  virtual void setLayout(int newLayout);

  /** This function is used to pass a function-pointer to the slider. This pointer has to be the 
  address of a function which has a double-parameter and a juce::String as return-value. The 
  function will be used to convert the slider-value into a string for display. */
  virtual void setStringConversionFunction(
    juce::String (*newConversionFunction) (double valueToBeConverted) );

  /** Switches the slider into vertical mode of operation - the default mode is horizontal. */
  //virtual void setVertical(bool shouldBeVertical);

  /** Sets up the range of the slider, a quantization interval for the values, a default value and 
  optionally initializes the current value to the default value. */
  virtual void setRange(double newMinimum, double newMaximum, double newInterval, 
    double newDefaultValue, bool initToDefault = true);

  /** Sets the scaling behavior of the slider (linear, exponential, etc.) 
  @see Parameter::scalings. */
  virtual void setScaling(int newScaling);

  /** Sets the current value of the slider and optionally sends out a callback message. */
  virtual void setValue(double newValue, const bool sendUpdateMessage = true, 
    const bool sendMessageSynchronously = false);

  /** Overriden from RWidget - sets the current value of the slider from a string and optionally 
  sends out a callback message. */
  virtual void setStateFromString(const juce::String &stateString, bool sendChangeMessage = true);

  /** Sets the value of the slider expressed as proportion of the slider's range, taking into 
  account the scaling behaviour. The value is thus between 0...1. */
  virtual void setProportionalValue(double newProportionalValue, 
    const bool sendUpdateMessage = true, const bool sendMessageSynchronously = false);
  // todo: rename to setNormalizedValue

  /** Sets the single default value that will be used on ctrl-click. */
  virtual void setDefaultValue(double newDefaultValue);

  /** Sets the default values that are accessible via right-click menu */
  virtual void setDefaultValues(juce::Array<double> newDefaultValue);

  /** Sets the current value of the slider to the default value and optionally sends out a callback 
  message.. */
  virtual void setToDefaultValue(const bool sendUpdateMessage = true, 
    const bool sendMessageSynchronously = false);

  /** Overrides a RWidget::assignParameter in order to retrieve some infos from the Parameter (such 
  as range, default-value, etc.) and sets up the slider accordingly. */
  virtual void assignParameter(Parameter* parameterToAssign);

  /** The callback method that will get called when one of our observed parameters has changed its 
  range. */
  virtual void parameterRangeChanged(Parameter* parameterThatHasChangedRange);

  /** Copies the settings (such as the range, scaling etc. but not the assignedParameter) from 
  another RSlider into this one.*/
  virtual void copySettingsFrom(const RSlider* otherSlider);

  //-----------------------------------------------------------------------------------------------
  // inquiry:

  /** Returns the name of the slider. */
  virtual const juce::String& getSliderName() const;

  /** Returns the current value of the slider. */
  virtual double getValue() const { return currentValue; };

  /** Returns the value of the slider expressed as proportion of the slider's range, taking into 
  account the scaling behaviour. The value is thus between 0...1. */
  virtual double getProportionalValue() const { return valueToProportionOfLength(currentValue); }

  /** Returns the maximum value of the slider. */
  virtual double getMaximum()  const { return maxValue; }

  /** Returns the minimum value of the slider. */
  virtual double getMinimum()  const { return minValue; }

  /** Returns the quantization interval for the values of the slider. */
  virtual double getInterval() const { return interval; }

  /** Converts a value representing a proportion of the sliders length (assumed to be in 0...1) to 
  the corresponding value of the slider. */
  virtual double proportionOfLengthToValue(double proportion) const;

  /** Converts a value of the slider into a value that represents a proportion of the sliders 
  length (in the range 0...1). */
  virtual double valueToProportionOfLength(double value) const;

  /** Overriden from RWidget - returns the state (defined as the current value) as string. */
  virtual juce::String getStateAsString() const;

  virtual const juce::String getTextFromValue(double value) const;

  virtual double getValueFromText(const juce::String& text) const;

  //-----------------------------------------------------------------------------------------------
  // callbacks:

  virtual void rPopUpMenuChanged(RPopUpMenu* menuThatHasChanged);

  /** Overrides mouseDown to update the slider according to the mouse-position. */
  virtual void mouseDown (const MouseEvent& e);

  /** Overrides mouseDrag to update the slider according to the mouse-position. */
  virtual void mouseDrag(const MouseEvent& e);

  /** Overrides mouseWheelMove in order adjust the slider on such events */
  virtual void mouseWheelMove(const MouseEvent& e, float wheelIncrementX, float wheelIncrementY);


  virtual void mouseDoubleClick (const MouseEvent& e);

  virtual void resized();

  /** Paints the slider. */
  virtual void paint(Graphics& g);

  //-----------------------------------------------------------------------------------------------
  // others:

  /** Adds a listener to this slider whcih will be called back when the slider's value has 
  changed. */
  virtual void addListener(RSliderListener* listener) throw();

  /** Removes a listener form this slider. */
  virtual void removeListener(RSliderListener* listener) throw();

  /** Overrides the method inherited from RWidget */
  virtual void updateWidgetFromAssignedParameter(bool sendChangeMessage = false);

protected:

  /** Enumeration of the identifiers to used as return-values for the right-click popup menu. */
  enum rightClickPopUpItemIdentifiers
  {
    ENTER_VALUE = 1,
    DEFAULT_VALUE,
    MIDI_LEARN,
    MIDI_MIN,
    MIDI_MAX,
    MIDI_REVERT
  };

  /** Clears the popup-menu and then calls createPopUpMenuItems() */
  virtual void updatePopUpMenu();

  /** Populates the right-click popup menu with items, according to the settings of this 
  RSlider. */
  virtual void addPopUpMenuItems();

  // called from createPopUpMenuItems:
  virtual void addPopUpEnterValueItem();
  virtual void addPopUpDefaultValueItems();
  virtual void addPopUpMidiItems();

  /** Opens the PopupMenu that appears on right clicks. */
  virtual void openRightClickPopupMenu();

  /** Opens a modal field for manually entering a value and returns the value entered. */
  virtual double openModalNumberEntryField();

  /** Returns a value that is constrained to the range of the slider. */
  virtual double constrainValue(double value) const throw();

  /** Returns a value that is constrained to the range of the slider and quantized to the 
  interval as set by setRange(). */
  virtual double constrainAndQuantizeValue(double value) const throw();

  /** Constrains the members currentValue and defaultValue via constrainedValue(). */
  virtual void valueSanityCheck();

  //virtual void enablementChanged();
  virtual void handleAsyncUpdate();
  virtual void triggerChangeMessage(const bool synchronous);

  // our listeners:
  juce::Array<RSliderListener*> listeners;  // rename into sliderObservers

  // internal state variables:
  double currentValue, minValue, maxValue, defaultValue, interval;
  juce::Array<double> defaultValues;
  int    scaling;
  int    layout;

  Rectangle<int> handleRectangle; 
  juce::String   sliderName;
  Component      *nameRectangle; // just a dummy in order to not receive mouse-events when the user clicks on the name-field
  RPopUpMenu     *rightClickPopUp;

private:

  // make copy constructor and assignment operator unavailable:
  RSlider(const RSlider&);
  const RSlider& operator= (const RSlider&);
  JUCE_LEAK_DETECTOR(RSlider);

};

#endif   
