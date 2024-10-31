#include "RButton.h"

// construction/destruction:

RButton::RButton(int newSymbolIndex, const Skin *skinToUse) 
: RWidget(skinToUse)
{
  symbolIndex       = newSymbolIndex;
  text              = String::empty;
  isOn              = false;
  clickTogglesState = true;
}

RButton::RButton(const String& buttonText, const Skin *skinToUse) 
: RWidget(skinToUse)
{
  symbolIndex       = 0;
  text              = buttonText;
  isOn              = false;
  clickTogglesState = true;
}

RButton::~RButton()
{

}

// setup:

void RButton::setSymbolIndex(int newSymbolIndex)
{
  symbolIndex = newSymbolIndex;
}

void RButton::setButtonText(const String& newText) throw()
{
  if(text != newText) // get rid of this conditional
  {
    text = newText;
    repaint();
  }
}

void RButton::setClickingTogglesState(const bool shouldToggle)
{
  clickTogglesState = shouldToggle;
}

void RButton::setToggleState(const bool shouldBeOn, const bool sendChangeNotification)
{
  if(shouldBeOn != isOn)
  {
    isOn = shouldBeOn;
    repaint();
    if(sendChangeNotification)
      sendClickMessage();
  }
}

void RButton::setStateFromString(const juce::String &stateString, bool sendChangeMessage)
{
  if( stateString.getIntValue() == 0 )
    setToggleState(false, sendChangeMessage);
  else
    setToggleState(true,  sendChangeMessage);
}

// inquiry:

juce::String RButton::getStateAsString() const
{
  if( getToggleState() == true )
    return "1";
  else
    return "0";
}

int RButton::getAdjacentButtonOffset() const
{
  return -getOutlineThickness();
}

// others:

void RButton::addRButtonListener (RButtonListener* const newListener) throw()
{
  jassert(newListener != 0);
  jassert(!buttonListeners.contains(newListener)); // trying to add a listener to the list twice!
  if(newListener != 0)
    buttonListeners.add(newListener);
}

void RButton::removeRButtonListener(RButtonListener* const listener) throw()
{
  jassert(buttonListeners.contains(listener)); // trying to remove a listener that isn't on the list!
  buttonListeners.removeValue(listener);
}

void RButton::sendClickMessage()
{
  for(int i=0; i<buttonListeners.size(); i++)
    buttonListeners[i]->rButtonClicked(this);
}

void RButton::mouseDown(const MouseEvent& e)
{
  clicked();
}

/*
void RButton::mouseEnter(const juce::MouseEvent &e)
{
  RWidget::mouseEnter(e);
  repaint();
}
void RButton::mouseExit(const MouseEvent &e)
{
  RWidget::mouseExit(e);
  repaint();
}
*/

void RButton::enablementChanged()
{
  repaint();
}

//void RButton::updateWidgetFromAssignedParameter(bool sendChangeMessage)
//{
//  if( assignedParameter != NULL )
//    setToggleState(assignedParameter->getValue() >= 0.5, sendChangeMessage);
//}

void RButton::clicked()
{
  if( clickTogglesState )
    isOn = !isOn;
  sendClickMessage();
  //if( assignedParameter != NULL )
  //  assignedParameter->setValue((double) getToggleState(), true, true);
  repaint();
}

void RButton::paint(Graphics &g)
{
  int w = getWidth();
  int h = getHeight();

  // draw background and outline:
  if( getToggleState() ) // || isDown() )
    g.fillAll(getMiddlegroundColor());
  else
    g.fillAll(getBackgroundColor());
  g.setColour(getOutlineColor());
  g.drawRect(0, 0, w, h, getOutlineThickness());

  // draw the text or symbol:
  g.setColour(getForegroundColor());
  if( symbolIndex <= 0 || symbolIndex > NUM_SYMBOLS )
  {
    g.setFont(getFont());
    g.drawText(getButtonText(), 0, 0, w, h, Justification::centred, false);
  }
  else
    drawSymbol(g);


  /*
  // old:
  // draw background and outline:
  if( getToggleState() ) // || isDown() )
    g.fillAll(skin->middlegroundColor);
  else
    g.fillAll(skin->backgroundColor);

  if( isMouseOver() )
    g.setColour(skin->outlineColorHighlight);
  else
    g.setColour(skin->outlineColor);
  g.drawRect(0, 0, w, h, skin->outlineThickness);

  // draw the text or symbol:
  g.setColour(skin->foregroundColor);
  if( symbolIndex <= 0 || symbolIndex > NUM_SYMBOLS )
  {
    g.setFont(skin->normalFont);
    g.drawText(getButtonText(), 0, 0, w, h, Justification::centred, false);
  }
  else
    drawSymbol(g);
    */

  // gray out the button if it's disabled:
  //if( !isEnabled() )
  //  g.fillAll(Colours::lightgrey.withAlpha(0.75f));
}

void RButton::drawSymbol(Graphics &g) const
{
  float w = (float) getWidth();
  float h = (float) getHeight(); 
  float m = 4.f;  // margin in pixels

  if( h >= 20.f )
    m = 5.f;

  float x1, x2, y1, y2;

  switch( symbolIndex )
  {
  case PLUS:
    {
      x1 = 0.5f*getWidth();
      x2 = x1;
      y1 = getHeight()-4.f;
      y2 = 4.f;
      g.drawLine(x1, y1, x2, y2, 2.f);

      x1 = 4.f;
      x2 = getWidth()-4.f;
      y1 = 0.5f*(getHeight());
      y2 = y1;
      g.drawLine(x1, y1, x2, y2, 2.f);
    }
    break;

  case MINUS:
    {
      x1 = 4.f;
      x2 = getWidth()-4.f;
      y1 = 0.5f*(getHeight());
      y2 = y1;
      g.drawLine(x1, y1, x2, y2, 2.f);
    }
    break;

  case ARROW_UP:
    {
      Path path;
      path.addTriangle(m, h-m, 0.5f*w, m, w-m, h-m);
      g.fillPath(path);
    }
    break;

  case ARROW_DOWN:
    {
      Path path;
      path.addTriangle(m, m, w-m, m, 0.5f*w, h-m);
      g.fillPath(path);
    }
    break;

  case ARROW_LEFT:
    {
      Path path;
      path.addTriangle(m, 0.5f*h, w-m, m, w-m, h-m);
      g.fillPath(path);
    }
    break;

  case ARROW_RIGHT:
    {
      Path path;
      path.addTriangle(m, m, m, h-m, w-m, 0.5f*h);
      g.fillPath(path);
    }
    break;

  case PLAY:
    {
      Path path;
      path.addTriangle(12.f, 8.f, getWidth()-12.f, 0.5f*getHeight(), 12.f, getHeight()-8.f);
      g.fillPath(path);
    }
    break;

  case SKIP_FORWARD:
    {
      Path path;
      path.addTriangle(8.f, 4.f, getWidth()-8.f, 0.5f*getHeight(), 8.f, getHeight()-4.f);
      path.addLineSegment(Line<float>(getWidth()-8.f, 4.f, getWidth()-8.f, getHeight()-4.f), 2.f);
      g.fillPath(path);
    }
    break;

  case SKIP_BACK:
    {
      Path path;
      path.addTriangle(getWidth()-8.f, 4.f, 8.f, 0.5f*getHeight(), getWidth()-8.f, getHeight()-4.f);
      path.addLineSegment(Line<float>(8.f, 4.f, 8.f, getHeight()-4.f), 2.f);
      g.fillPath(path);
    }
    break;

  case CLOSE:
    {
      g.drawLine(                  4.f, 4.f, (float)getWidth()-4.f, (float)getHeight()-4.f, 2.f);
      g.drawLine((float)getWidth()-4.f, 4.f,                   4.f, (float)getHeight()-4.f, 2.f);
    }
    break;
  }
}


//=================================================================================================
// class RClickButton:

RClickButton::RClickButton(int newSymbolIndex, const Skin *skinToUse) 
: RButton(newSymbolIndex, skinToUse)
{
  clickTogglesState = false;
}

RClickButton::RClickButton(const juce::String& buttonText, const Skin *skinToUse) 
: RButton(buttonText, skinToUse)
{
  clickTogglesState = false;
}

void RClickButton::mouseDown(const MouseEvent& e)
{
  isOn = true;
  sendClickMessage();
  repaint();
}

void RClickButton::mouseUp(const MouseEvent& e)
{
  isOn = false;
  //sendClickMessage();
  repaint();
}


//=================================================================================================
// class RClickButtonNotifyOnMouseUp:

RClickButtonNotifyOnMouseUp::RClickButtonNotifyOnMouseUp(int newSymbolIndex, const Skin *skinToUse) 
: RClickButton(newSymbolIndex, skinToUse)
{

}

RClickButtonNotifyOnMouseUp::RClickButtonNotifyOnMouseUp(const juce::String& buttonText, 
  const Skin *skinToUse) 
: RClickButton(buttonText, skinToUse)
{

}

void RClickButtonNotifyOnMouseUp::mouseDown(const MouseEvent& e)
{
  isOn = true;
  repaint();
}

void RClickButtonNotifyOnMouseUp::mouseUp(const MouseEvent& e)
{
  if( isOn && contains(Point<int>(e.x, e.y)) )
    sendClickMessage();
  isOn = false;
  repaint();
}


//=================================================================================================
// class RClickButtonWithAutoRepeat:

RClickButtonWithAutoRepeat::RClickButtonWithAutoRepeat(int newSymbolIndex, const Skin *skinToUse) 
: RClickButton(newSymbolIndex, skinToUse)
{
  initialDelay = 200;
  timeInterval = 50;
}

RClickButtonWithAutoRepeat::RClickButtonWithAutoRepeat(const juce::String& buttonText, 
  const Skin *skinToUse) 
: RClickButton(buttonText, skinToUse)
{

}

void RClickButtonWithAutoRepeat::mouseDown(const MouseEvent& e)
{
  isOn = true;
  repaint();
  sendClickMessage();
  startTimer(initialDelay);
}

void RClickButtonWithAutoRepeat::mouseUp(const MouseEvent& e)
{
  isOn = false;
  repaint();
  stopTimer();
}

void RClickButtonWithAutoRepeat::timerCallback()
{
  sendClickMessage();
  startTimer(timeInterval);
}


//=================================================================================================
// class RRadioButton:

RRadioButton::RRadioButton(int newSymbolIndex, const Skin *skinToUse) 
: RButton(newSymbolIndex, skinToUse)
{
  radioGroupToUse = NULL;
}

RRadioButton::RRadioButton(const juce::String& buttonText, const Skin *skinToUse) 
: RButton(buttonText, skinToUse)
{
  radioGroupToUse = NULL;
}

void RRadioButton::addToRadioButtonGroup(RRadioButtonGroup *newGroupToUse)
{
  if( radioGroupToUse != NULL )
    radioGroupToUse->removeButtonFromRadioGroup(this);

  radioGroupToUse = newGroupToUse;

  if( radioGroupToUse != NULL )
    radioGroupToUse->addButtonToRadioGroup(this);
}

void RRadioButton::clicked()
{
  setToggleState(!isOn, true);
}


void RRadioButton::setToggleState(const bool shouldBeOn, const bool sendNotifications)
{
  jassert( radioGroupToUse != NULL ); // forgotten to assign this radio-button to a radio-group?
  if( radioGroupToUse != NULL )
  {
    if( shouldBeOn )
      radioGroupToUse->toggleRadioButtonOn(this, sendNotifications);
  }
}

void RRadioButtonGroup::addButtonToRadioGroup(RRadioButton *buttonToAdd)
{
  radioButtons.addIfNotAlreadyThere(buttonToAdd);
}

void RRadioButtonGroup::removeButtonFromRadioGroup(RRadioButton *buttonToRemove)
{
  radioButtons.removeValue(buttonToRemove);
}

void RRadioButtonGroup::toggleRadioButtonOn(RRadioButton *buttonToToggleOn, bool sendNotifications)
{
  jassert( radioButtons.contains(buttonToToggleOn) ); // button does not belong into this group
  for(int i=0; i<radioButtons.size(); i++)
  {
    RButton *button = (RButton*) radioButtons[i];   // upcast invokes baseclass'es method
    if( radioButtons[i] == buttonToToggleOn )
      button->setToggleState(true, sendNotifications);
    else
      button->setToggleState(false, sendNotifications);
  }
}
