#include "RWidget.h"

RWidget::RWidget(const Skin *skinToUse, const String& newDescription) 
: DescribedComponent(newDescription)
{
  jassert(skinToUse != nullptr); // you should pass a valid pointer to a Skin object

  stringConversionFunction = &valueToString2;  // function pointer for string conversion
  assignedParameter        = nullptr;
  highlightLevel           = HIGHLIGHT_NONE;
  fontSize                 = FONT_NORMAL;
  skin                     = skinToUse;
}


void RWidget::assignParameter(Parameter *parameterToAssign)
{
  if( assignedParameter != nullptr )
    assignedParameter->deRegisterParameterObserver(this);
  assignedParameter = parameterToAssign;
  if( assignedParameter != nullptr )
    assignedParameter->registerParameterObserver(this);
}
void RWidget::setFontSize(int newSize)
{
  fontSize = newSize;
}

// \todo get rid of the code-duplication below, perhaps by factoring out a ColorTriple class
// which defines normalColor, semiHighlightColor and highlightColor:
Colour RWidget::getBackgroundColor() const
{
  switch( highlightLevel )
  {
  case HIGHLIGHT_SEMI: return skin->backgroundColorSemiHighlight;
  case HIGHLIGHT_FULL: return skin->backgroundColorHighlight;
  default:             return skin->backgroundColor;
  }
  /*
  if( isMouseButtonDown() )
    return skin->backgroundColorHighlight;
  else if( isMouseOver() )
    return skin->backgroundColorSemiHighlight;
  else
    return skin->backgroundColor;
    */
}
Colour RWidget::getMiddlegroundColor() const
{
  switch( highlightLevel )
  {
  case HIGHLIGHT_SEMI: return skin->middlegroundColorSemiHighlight;
  case HIGHLIGHT_FULL: return skin->middlegroundColorHighlight;
  default:             return skin->middlegroundColor;
  }
  /*
  if( isMouseButtonDown() )
    return skin->middlegroundColorHighlight;
  else if( isMouseOver() )
    return skin->middlegroundColorSemiHighlight;
  else
    return skin->middlegroundColor;
   */
}
Colour RWidget::getForegroundColor() const
{
  switch( highlightLevel )
  {
  case HIGHLIGHT_SEMI: return skin->foregroundColorSemiHighlight;
  case HIGHLIGHT_FULL: return skin->foregroundColorHighlight;
  default:             return skin->foregroundColor;
  }
  /*
  if( isMouseButtonDown() )
    return skin->foregroundColorHighlight;
  else if( isMouseOver() )
    return skin->foregroundColorSemiHighlight;
  else
    return skin->foregroundColor;
  */
}
Colour RWidget::getOutlineColor() const
{
  switch( highlightLevel )
  {
  case HIGHLIGHT_SEMI: return skin->outlineColorSemiHighlight;
  case HIGHLIGHT_FULL: return skin->outlineColorHighlight;
  default:             return skin->outlineColor;
  }
  /*
  if( isMouseButtonDown() )
    return skin->outlineColorHighlight;
  else if( isMouseOver() )
    return skin->outlineColorSemiHighlight;
  else
    return skin->outlineColor;
  */
}
int RWidget::getOutlineThickness() const
{
  return skin->outlineThickness;
}
const Font& RWidget::getFont() const
{
  switch( fontSize )
  {
  case FONT_SMALL: return skin->smallFont;
  case FONT_LARGE: return skin->bigFont;
  default:         return skin->normalFont;
  }
}
const Skin* RWidget::getSkin() const
{
  return skin;
}

// callbacks:

void RWidget::mouseEnter(const MouseEvent &e)
{
  DescribedComponent::mouseEnter(e);
  highlightLevel = HIGHLIGHT_SEMI;
  repaint();
}
void RWidget::mouseExit(const MouseEvent &e)
{
  DescribedComponent::mouseExit(e);
  highlightLevel = HIGHLIGHT_NONE;
  repaint();
}
void RWidget::mouseDown(const MouseEvent &e)
{
  highlightLevel = HIGHLIGHT_FULL;
  repaint();
}
void RWidget::mouseUp(const MouseEvent &e)
{
  if( isMouseOver() )
    highlightLevel = HIGHLIGHT_SEMI;
  else
    highlightLevel = HIGHLIGHT_NONE;
  repaint();
}





void RWidget::parameterChanged(Parameter* parameterThatHasChanged)
{
  triggerAsyncUpdate();
}

void RWidget::parameterRangeChanged(Parameter* parameterThatHasChanged)
{
  triggerAsyncUpdate();
}

void RWidget::parameterIsGoingToBeDeleted(Parameter* parameterThatWillBeDeleted)
{
  if( assignedParameter == parameterThatWillBeDeleted )
  {
    assignedParameter->deRegisterParameterObserver(this);
    assignedParameter = NULL;
  }
}

void RWidget::handleAsyncUpdate()
{
  if( assignedParameter != NULL )
  {
    localAutomationSwitch = false; // to avoid circular notifications and updates
    updateWidgetFromAssignedParameter();
    localAutomationSwitch = true;
  }
}

void RWidget::updateWidgetFromAssignedParameter(bool sendChangeMessage)
{
  // needs to be overriden in the subclasses to - for example - update a slider like this:
  // if( assignedParameter != NULL )
  // {
  //  setValue(assignedParameter->getValue());
  // }
}