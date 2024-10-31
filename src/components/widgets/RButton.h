#ifndef RButton2_h
#define RButton2_h

// \todo rename to MButton and get rid of the old implementation

//#include "MWidget.h"
#include "RWidget.h"

class RButton;

/** A class to receive callbacks when a button is clicked. */
class RButtonListener
{

public:

  /** Called when the button is clicked. */
  virtual void rButtonClicked(RButton* button) = 0;

  JUCE_LEAK_DETECTOR(RButtonListener);
};


/**

This class ....

*/

class RButton : public RWidget
{

public:

  enum buttonSymbols
  {
    NO_SYMBOL = 0,
    PLUS,
    MINUS,
    ARROW_UP,
    ARROW_DOWN,
    ARROW_LEFT,
    ARROW_RIGHT,
    PLAY,
    SKIP_FORWARD,
    SKIP_BACK,
    MUTE,
    LOOP,
    CLOSE,

    NUM_SYMBOLS
  };

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructs a button with a symbol. */
  RButton(int newSymbolIndex, const Skin *skinToUse);    

  /** Constructs a button with text. */
  RButton(const String& buttonText, const Skin *skinToUse);

  /** Destructor. */
  virtual ~RButton();

  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Chooses a new symbol for this button. */
  virtual void setSymbolIndex(int newSymbolIndex);

  /** Changes the button's text. */
  void setButtonText(const juce::String& newText) throw();

  /** Decides, whether or not this button should change its on/off state when clicked. */
  void setClickingTogglesState(const bool shouldToggle);

  /** A button has an on/off state associated with it, and this changes that. By default buttons are 'off' and for simple buttons that 
  you click to perform an action you won't change this. Toggle buttons, however will want to change their state when turned on or off. */
  void setToggleState(const bool shouldBeOn, const bool sendChangeNotification);

  /** Overriden from MWidget - sets the toggle-state to "off" when the string-as-integer is 0 and sets it to "on" when the 
  string-as-integer is != 0 (presumably "1"). */
  virtual void setStateFromString(const juce::String &stateString, bool sendChangeMessage = true);

  //-----------------------------------------------------------------------------------------------
  // inquiry:

  /** Returns the text displayed in the button. */
  const juce::String getButtonText() const throw() { return text; }

  /** Returns true if the button in 'on'. */
  bool getToggleState() const throw() { return isOn; }

  /** Returns "1" when the toggle-state is "on", otherwise "0". */
  virtual juce::String getStateAsString() const;

  /** Returns an off set in pixels for a button that should be adjacent to this. */
  virtual int getAdjacentButtonOffset() const;

  //-------------------------------------------------------------------------------------------------------------------------------------
  // others:

  /** Registers a listener to receive events when this button's state changes. */
  void addRButtonListener(RButtonListener* const newListener) throw();

  /** Removes a previously-registered button listener */
  void removeRButtonListener(RButtonListener* const listener) throw();

  /** Overrides the method inherited from MWidget */
  //virtual void updateWidgetFromAssignedParameter(bool sendChangeMessage = false);

  /** Paints the button. */
  virtual void paint(Graphics &g);

  /** @internal */
  virtual void mouseDown(const MouseEvent& e);

  // these should later be factored out to MWidget:
  //virtual void mouseEnter(const MouseEvent &e);
  //virtual void mouseExit(const MouseEvent &e);

  /** @internal */
  virtual void enablementChanged();

protected:

  /** Sends out a message to our listeners that this button has been clicked. */
  void sendClickMessage();

  /** Overrides the inherited clicked callback in order to update an assigned Parameter (if any). */
  virtual void clicked();

  /** Draws the symbol onto the button (if any). */
  virtual void drawSymbol(Graphics &g) const;


  juce::String text;
  juce::Array<RButtonListener*> buttonListeners;

  bool isOn;
  bool clickTogglesState;
  int  symbolIndex;

private:

  RButton(const RButton&);
  const RButton& operator=(const RButton&);
  JUCE_LEAK_DETECTOR(RButton);

};


//=================================================================================================
// class RClickButton: 

/** 

This class implements a button that paints as active when mouse is down, and switches back into 
inactive state when the mouse is up again, i.e. it doesn't toggle its state on clicking.

*/

class RClickButton : public RButton
{
public:
  RClickButton(int newSymbolIndex, const Skin *skinToUse);
  RClickButton(const juce::String& buttonText, const Skin *skinToUse);
  virtual void mouseDown(const MouseEvent& e);
  virtual void mouseUp(const MouseEvent& e);
   
  JUCE_LEAK_DETECTOR(RClickButton);
};


//=======================================================================================================================================
// class RClickButtonNotifyOnMouseUp: 

/** 

This class is like an RClickButton but it sends out a click message only on mouse-up events. Moreover, these mouse-up events must occur 
inside this button after a mouse-down event occured. This behaviour is desirable for OK/Cancel buttons on dialog boxes, for example. 

*/

class RClickButtonNotifyOnMouseUp : public RClickButton
{
public:
  RClickButtonNotifyOnMouseUp(int newSymbolIndex, const Skin *skinToUse);
  RClickButtonNotifyOnMouseUp(const juce::String& buttonText, const Skin *skinToUse);
  virtual void mouseDown(const MouseEvent& e);
  virtual void mouseUp(const MouseEvent& e);

  JUCE_LEAK_DETECTOR(RClickButtonNotifyOnMouseUp);
};


//=======================================================================================================================================
// class RClickButtonWithAutoRepeat 

/** 

This class is like an RClickButton but it sends out click messages repeatedly as long as it is clicked.

*/

class RClickButtonWithAutoRepeat : public RClickButton, public Timer
{
public:
  RClickButtonWithAutoRepeat(int newSymbolIndex, const Skin *skinToUse);
  RClickButtonWithAutoRepeat(const juce::String& buttonText, const Skin *skinToUse);
  virtual void mouseDown(const MouseEvent& e);
  virtual void mouseUp(const MouseEvent& e);
  virtual void timerCallback();

protected:
  int initialDelay, timeInterval; // both values in ms \todo maybe provide setters and getters for these
  JUCE_LEAK_DETECTOR(RClickButtonWithAutoRepeat);
};


//=======================================================================================================================================
// class RRadioButton and RRadioButtonGroup: 

class RRadioButtonGroup;

/** 

This class implements a button that can be used in a group of mutually exclusively pressed buttons - which means only one at a time can 
be in 'pressed' state (this kind of behavior is also known as "radio-button").

*/

class RRadioButton : public RButton
{

  //friend class RRadioButtonGroup;

public:

  RRadioButton(int newSymbolIndex, const Skin *skinToUse);

  RRadioButton(const juce::String& buttonText, const Skin *skinToUse);

  /** Sets the radio-group of which this button should become a member. */
  virtual void addToRadioButtonGroup(RRadioButtonGroup *newGroupToUse);

  /** Overriden clicked to make sure that all other buttons in the same radio-group are going to be siwtched off. */
  virtual void clicked();

  /** Overriden to make sure that all other buttons in the same radio-group are going to be siwtched off. */
  void setToggleState(const bool shouldBeOn, const bool sendChangeNotification);

protected:

  RRadioButtonGroup *radioGroupToUse;
  JUCE_LEAK_DETECTOR(RRadioButton);
};


class RRadioButtonGroup
{
public:
  virtual void addButtonToRadioGroup(RRadioButton *buttonToAdd);
  virtual void removeButtonFromRadioGroup(RRadioButton *buttonToRemove);

  /** Toggles the passed button (which is assumed to be a member of this group) on and toggles all other buttons in the group off. */
  virtual void toggleRadioButtonOn(RRadioButton *buttonToToggleOn, bool sendNotifications);

protected:
  juce::Array<RRadioButton*> radioButtons;
  JUCE_LEAK_DETECTOR(RRadioButtonGroup);
};


#endif  
