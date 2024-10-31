#include "RTextField.h"

RTextField::RTextField(const Skin *skinToUse, const juce::String& initialText) 
: RWidget(skinToUse)
, justification(Justification::centredLeft)
{ 
  text                   = initialText; 
  justification          = Justification::centredLeft;
  noBackgroundAndOutline = true;
}

void RTextField::setText(const juce::String &newText) 
{ 
  text = newText; 
  repaint();
}
   
void RTextField::setJustification(const Justification& newJustification)
{
  justification = newJustification;
  repaint();
}

void RTextField::setStateFromString(const juce::String &stateString, bool sendChangeMessage)
{
  setText(stateString);
}

void RTextField::setNoBackgroundAndOutline(bool shouldBeWithout)
{
  noBackgroundAndOutline = shouldBeWithout;
  repaint();
}

int RTextField::getTextPixelPositionX() const
{
  int hFlags = justification.getOnlyHorizontalFlags();
  float tw   = getFont().getStringWidthFloat(text);
  float x;

  if( justification.testFlags(hFlags & Justification::centredLeft) )
    x = horizontalMargin;
  else if( justification.testFlags(hFlags & Justification::centred) )
    x = getWidth()/2 - tw/2;
  else if( justification.testFlags(hFlags & Justification::centredRight) )
    x = getWidth() - tw - horizontalMargin;

  return (int)x;
}

int RTextField::getTextPixelPositionY() const
{
  return (int) (0.5f*getHeight() - 0.5f*getFont().getAscent());
}

int RTextField::getWidthToFitText() const
{
  float tw = getFont().getStringWidthFloat(text);
  return (int) ceil(tw + 2*getOutlineThickness() + 2*horizontalMargin);

  //return boldFont10px.getTextPixelWidth(text, boldFont10px.getDefaultKerning()) + 
  //  2*outlineThickness + 2*horizontalMargin;
}


juce::String RTextField::getStateAsString() const
{
  return getText();
}

void RTextField::assignParameter(Parameter* parameterToAssign)
{
  RWidget::assignParameter(parameterToAssign);
  updateWidgetFromAssignedParameter(false);
}

void RTextField::parameterChanged(Parameter* parameterThatHasChanged)
{
  if( assignedParameter != NULL && parameterThatHasChanged == assignedParameter )
    setText(String(assignedParameter->getValue()));
}

void RTextField::updateWidgetFromAssignedParameter(bool sendChangeMessage)
{
  if( assignedParameter != NULL )
    setText(String(assignedParameter->getValue()));
}

void RTextField::paint(Graphics &g)
{
  if( noBackgroundAndOutline == false )
    RWidget::paint(g);

  g.setFont(getFont());  // use getFont()
  g.setColour(getForegroundColor());

  g.drawText(text, getTextPixelPositionX(), getTextPixelPositionY(), getWidth(), getHeight(), 
    justification, false);

  //drawBitmapFontText(g, getTextPixelPositionX(), getTextPixelPositionY(), text, &boldFont10px, getTextColour());
}


//=================================================================================================
// class RTextEntryField:

RTextEntryField::RTextEntryField(const Skin *skinToUse, const juce::String& initialText) 
: RTextField(skinToUse, initialText)
{
  noBackgroundAndOutline = false;
  caretPosition          = 0;
  selectionStart         = 0;
  selectionEnd           = 0; 
  caretVisible           = false;
  replaceMode            = false;
  textInvalid            = false;
  oldText                = text;
  setWantsKeyboardFocus(true);
}

void RTextEntryField::setPermittedCharacters(const String& newCharacters, bool deleteNonPermittedCharsFromText)
{
  permittedCharacters = newCharacters;
  if( deleteNonPermittedCharsFromText )
  {
    deSelect();
    caretPosition = 0;
    juce::String oldText = text;
    text = text.retainCharacters(permittedCharacters);
    if( text != oldText )
    {
      sendTextChangeNotificationIfChanged();
      repaint();
    }
  }
}

void RTextEntryField::registerTextEntryFieldObserver(RTextEntryFieldObserver *observerToRegister)
{
  textEntryFieldObservers.add(observerToRegister);
}

void RTextEntryField::deRegisterTextEntryFieldObserver(RTextEntryFieldObserver *observerToDeRegister)
{
  textEntryFieldObservers.removeValue(observerToDeRegister);
}

void RTextEntryField::selectAll()
{
  selectionStart = 0;
  selectionEnd   = text.length();
}
 
void RTextEntryField::deSelect()
{
  selectionStart = 0;
  selectionEnd   = 0;
}

void RTextEntryField::markTextAsInvalid(bool shouldBeMarkedInvalid)
{
  textInvalid = shouldBeMarkedInvalid;
  repaint();
}

Point<int> RTextEntryField::getCaretPixelPosition() const
{
  int x = characterIndexToPixelPosition(caretPosition);
  int y = 2;
  return Point<int>(x, y);
}

int RTextEntryField::characterIndexToPixelPosition(int index) const
{
  juce::String subString = text.substring(0, caretPosition);
  //int x = horizontalMargin + boldFont10px.getTextPixelWidth(subString, boldFont10px.getDefaultKerning());
  int x = horizontalMargin + getFont().getStringWidth(subString);
  return x;
}

int RTextEntryField::pixelPositionToCharacterIndex(int pixelX) const
{
  Array<int>   glyphs;
  Array<float> xOffsets;
  getFont().getGlyphPositions(text, glyphs, xOffsets);
  int   i = 0;
  float x = horizontalMargin;
  float w = 0.f;
  while( x < (float) pixelX && i < xOffsets.size() )
  {
    x = horizontalMargin + xOffsets[i];
    i++;
  }
  i = jmax(i-1, 0);
  if( i < xOffsets.size()-1 )
  {
    w = xOffsets[i+1] - xOffsets[i];
    if( x-pixelX > w/2 )
      i--;
  }
  return jmax(i, 0);





  /*
  // old:
  int i = 0;
  float x = horizontalMargin;
  float w = 0.f;
  while( x < pixelX && i < text.length() )
  {
    //w  = boldFont10px.getGlyphWidth(text[i]) + boldFont10px.getDefaultKerning();
    w = getFont().getStringWidthFloat(String(text[i]));
    x += w;
    i++;
  }
  if( i > 0 && x-pixelX > w/2 )
    i--; // move one character leftward if we are before the midpoint
  return jlimit(0, text.length(), i);
  */

  // \todo: use Font::getGlyphPositions
}

String RTextEntryField::getTextBeforeSelection() const
{
  return text.substring(0, selectionStart);
}

String RTextEntryField::getTextInsideSelection() const
{
  return text.substring(selectionStart, selectionEnd);
}

String RTextEntryField::getTextAfterSelection() const
{
  return text.substring(selectionEnd, text.length());
}

void RTextEntryField::getHeadAndTailString(String &head, String &tail) const
{
  if( isSelectionEmpty() )
  {
    head = text.substring(0, caretPosition);
    tail = text.substring(caretPosition);
  }
  else
  {
    head = getTextBeforeSelection();
    tail = getTextAfterSelection();
  }  
}

void RTextEntryField::setStateFromString(const juce::String &stateString, bool sendChangeMessage)
{
  RTextField::setText(stateString);
  if( sendChangeMessage == true )
    sendTextChangeNotificationIfChanged();
}

void RTextEntryField::inputAttemptWhenModal()
{
  text = oldText;
  if( isCurrentlyModal() )
    exitModalState(0);
}

bool RTextEntryField::keyPressed(const KeyPress &key)
{
  String head, tail;
  if( key.getModifiers().isCommandDown() )
  {
    if( key == KeyPress('c', ModifierKeys::commandModifier, 0) )
    {     
      if( !isSelectionEmpty() )
        SystemClipboard::copyTextToClipboard(getTextInsideSelection());
      return true;
    }
    else if( key == KeyPress('v', ModifierKeys::commandModifier, 0) )
    {
      getHeadAndTailString(head, tail);
      String stringToInsert(SystemClipboard::getTextFromClipboard());
      text          = head + stringToInsert + tail;
      caretPosition = head.length() + stringToInsert.length();
      sendTypingNotification();
      repaint();
    }
  }
  else if( key == KeyPress::insertKey )
  {
    replaceMode = !replaceMode;
  }
  else if( key == KeyPress::returnKey )
  {
    sendTextChangeNotificationIfChanged();
    if( isCurrentlyModal() )
      exitModalState(0);
  }
  else if( key == KeyPress::escapeKey )
  {
    text = oldText;
    if( isCurrentlyModal() )
      exitModalState(0);
  }
  else if( key == KeyPress::backspaceKey )
  {
    getHeadAndTailString(head, tail);
    if( isSelectionEmpty() )
      head = head.substring(0, head.length()-1);
    text = head + tail;
    caretPosition = head.length();
    sendTypingNotification();
    repaint();
  }
  else if( key == KeyPress::deleteKey )
  {
    getHeadAndTailString(head, tail);
    if( isSelectionEmpty() )
      tail = tail.substring(1, tail.length());
    text = head + tail;
    caretPosition = head.length();
    sendTypingNotification();
    repaint();
  }
  else if( key == KeyPress::leftKey )
  {
    caretPosition--;
    caretPosition = jmax(0, caretPosition);
    repaint();
  }
  else if( key == KeyPress::rightKey )
  {
    caretPosition++;
    caretPosition = jmin(caretPosition, text.length());
    repaint();
  }
  else if( key == KeyPress::homeKey )
  {
    caretPosition = 0;
    repaint();
  }
  else if( key == KeyPress::endKey )
  {
    caretPosition = text.length();
    repaint();
  }
  else
  {
    juce_wchar ch = key.getTextCharacter();
    if( permittedCharacters != String::empty && !permittedCharacters.containsChar(ch) )
      return true; // character is not among the permitted ones 
    getHeadAndTailString(head, tail);
    if( isSelectionEmpty() && replaceMode == true )
      tail = tail.substring(1, tail.length());
    text = head + ch + tail;
    caretPosition = head.length() + 1;
    sendTypingNotification();
    repaint();
  }
  deSelect();
  return true; // KeyPress was consumed and will not be passed any further to possibly registered KeyListeners
}

void RTextEntryField::focusGained(FocusChangeType cause)
{
  caretVisible = true;
  startTimer(blinkInterval);
  repaint();
}

void RTextEntryField::focusLost(FocusChangeType cause)
{
  caretVisible = false;
  stopTimer();
  sendTextChangeNotificationIfChanged();
  repaint();
  if( isCurrentlyModal() )  
    exitModalState(0);
}

void RTextEntryField::timerCallback()
{
  caretVisible = !caretVisible;
  startTimer(blinkInterval);
  repaint();
}

void RTextEntryField::mouseDown(const MouseEvent &e)
{  
  caretPosition = pixelPositionToCharacterIndex(e.x);
  deSelect();
}

void RTextEntryField::mouseDoubleClick(const MouseEvent &e)
{  
  caretPosition = pixelPositionToCharacterIndex(e.x);
  selectAll();
}

void RTextEntryField::mouseDrag(const MouseEvent &e)
{  
  int x1 = e.getMouseDownX();
  int x2 = x1 + e.getDistanceFromDragStartX();

  if( x1 > x2 )
    swapVariables(x1, x2);

  selectionStart = pixelPositionToCharacterIndex(x1);
  selectionEnd   = pixelPositionToCharacterIndex(x2);
}

void RTextEntryField::paint(juce::Graphics &g)
{
  if( !hasKeyboardFocus(false) )
    RTextField::paint(g);  
  else
  {
    if( isSelectionEmpty() )
    {
      RTextField::paint(g);  
      if( caretVisible )
      {
        Point<int> p = getCaretPixelPosition();
        int caretWidth  = 2;
        int caretHeight = getHeight()-4;
        if( caretPosition < text.length() && replaceMode == true )
        {
          //caretWidth = boldFont10px.getGlyphWidth(text[caretPosition]) + 1;
          caretWidth = getFont().getStringWidth(String(text[caretPosition]));
        }
        g.fillRect(p.getX(), p.getY(), caretWidth, caretHeight);
      }
    }
    else
    {
      if( noBackgroundAndOutline == false )
        RWidget::paint(g);
      int x = getTextPixelPositionX();
      int y = getTextPixelPositionY();
      g.setColour(getForegroundColor());
      g.setFont(getFont());
      g.drawText(getTextBeforeSelection(), x, y, getWidth(), getHeight(), justification, false);
      x += getFont().getStringWidth(getTextBeforeSelection());
      int xR = x + getFont().getStringWidth(getTextInsideSelection());

      g.setColour(getMiddlegroundColor());
      g.fillRect(x, 2, xR-x, getHeight()-4);

      //x = drawBitmapFontText(g, x, y, getTextInsideSelection(), &boldFont10px, tc);
      g.drawText(getTextInsideSelection(), x, y, getWidth(), getHeight(), justification, false);
      x += getFont().getStringWidth(getTextInsideSelection());

      //x = drawBitmapFontText(g, x, y, getTextAfterSelection(),  &boldFont10px, tc);
      g.drawText(getTextAfterSelection(), x, y, getWidth(), getHeight(), justification, false);

      /*
      // old:
      if( noBackgroundAndOutline == false )
        RWidget::paint(g);
      int x     = getTextPixelPositionX();
      int y     = getTextPixelPositionY();
      Colour tc = getTextColour();
      x         = drawBitmapFontText(g, x, y, getTextBeforeSelection(), &boldFont10px, tc);
      int xR    = x + boldFont10px.getTextPixelWidth(getTextInsideSelection(), boldFont10px.getDefaultKerning());
      g.setColour(getHandleColour());
      g.fillRect(x, 2, xR-x, getHeight()-4);
      x      = drawBitmapFontText(g, x, y, getTextInsideSelection(), &boldFont10px, tc);
      x      = drawBitmapFontText(g, x, y, getTextAfterSelection(),  &boldFont10px, tc);
      */
    }
  }

  if( textInvalid )
  {
    g.setColour(Colours::red.withAlpha(0.375f));  // maybe, use a color from the skin as well
    Rectangle<int> r(getOutlineThickness(), getOutlineThickness(), 
      getWidth()-2*getOutlineThickness(), getHeight()-2*getOutlineThickness());
    g.fillRect(r);
  }
}

void RTextEntryField::sendTextChangeNotificationIfChanged()
{
  if( text != oldText )
  {

    // maybe factor this out into an updateassignedParameter function:
    if( assignedParameter != NULL )
    {
      if( assignedParameter->isStringParameter() )
        assignedParameter->setStringValue(text, true);
      else
        assignedParameter->setValue(text.getDoubleValue(), true);
    }


    for(int i=0; i<textEntryFieldObservers.size(); i++)
      textEntryFieldObservers[i]->textChanged(this);
    oldText = text;
  }
}
    
void RTextEntryField::sendTypingNotification()
{
  for(int i=0; i<textEntryFieldObservers.size(); i++)
    textEntryFieldObservers[i]->somethingWasTypedIn(this);
}


//=========================================================================================================================================
// class RLabeledTextEntryField:

RLabeledTextEntryField::RLabeledTextEntryField(const Skin *skinToUse, const juce::String& labelText, 
  const juce::String& entryFieldText)
: RWidget(skinToUse)
{
  //setNoBackgroundAndOutline(true);

  labelField = new RTextField(skinToUse, labelText);
  labelField->setNoBackgroundAndOutline(true);

  entryField = new RTextEntryField(skinToUse, entryFieldText);

  labelWidth = 60;
}

RLabeledTextEntryField::~RLabeledTextEntryField()
{
  deleteAllChildren();  //necessary?
}

void RLabeledTextEntryField::setDescription(const juce::String &newDescription)
{
  labelField->setDescription(newDescription);
  entryField->setDescription(newDescription);
}

void RLabeledTextEntryField::setLabelWidth(int newWidth)
{
  labelWidth = newWidth;
  resized();
}

void RLabeledTextEntryField::setStateFromString(const juce::String &stateString, bool sendChangeMessage)
{
  entryField->setStateFromString(stateString, sendChangeMessage);
}


void RLabeledTextEntryField::resized()
{
  int w = getWidth();
  int h = getHeight();
  int entryWidth = w-labelWidth;
  labelField->setBounds(0,          0, labelWidth, h);
  entryField->setBounds(labelWidth, 0, entryWidth, h);
}



