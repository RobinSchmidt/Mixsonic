#ifndef RWidget_h
#define RWidget_h

#include "../../core/infrastructure/DescribedItem.h"
#include "../../control/Skin.h"
#include "../../control/Parameter.h"

/**

This class serves as base class for various GUI widgets. 

\todo maybe have (integer) members highlightLevel (HIGHLIGHT_NONE, HIGHLIGHT_SEMI, HIGHLIGHT_FULL)
and fontSize (FONT_SMALL, FONT_NORMAL, FONT_LARGE)

*/

class RWidget : public DescribedComponent, public ParameterObserver, public AsyncUpdater 
{

public:

  /** Possible values for our highlightLevel member. */
  enum highlightLevels
  {
    HIGHLIGHT_NONE, 
    HIGHLIGHT_SEMI, 
    HIGHLIGHT_FULL
  };

  /** Possible values for our fontSize member. */
  enum fontSizes
  {
    FONT_SMALL, 
    FONT_NORMAL, 
    FONT_LARGE
  };



  /** Constructor. */
  RWidget(const Skin *skinToUse, const String& newDescription = String("Widget"));
    // get rid of the description parameter



  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Assigns a Parameter object for observation and manipulation. */  
  virtual void assignParameter(Parameter* parameterToAssign);

  /** Sets the fontSize to be used - should be one of the values defined in the enum fontSizes. */
  virtual void setFontSize(int newSize);

  //-----------------------------------------------------------------------------------------------
  // inquiry:

  virtual Colour getBackgroundColor() const;
  virtual Colour getMiddlegroundColor() const;
  virtual Colour getForegroundColor() const;
  virtual Colour getOutlineColor() const;

  virtual int getOutlineThickness() const;


  virtual const Font& getFont() const;

  virtual const Skin* getSkin() const;


  virtual Parameter* getAssignedParameter() const { return assignedParameter; }

  //-----------------------------------------------------------------------------------------------
  // callbacks:


  virtual void mouseEnter(const MouseEvent &e);
  virtual void mouseExit(const MouseEvent &e);
  virtual void mouseDown(const MouseEvent &e);
  virtual void mouseUp(const MouseEvent &e);


  /** Overrides the purely virtual parameterChanged() method inherited from ParameterObserver. */
  virtual void parameterChanged(Parameter* parameterThatHasChanged);

  /** Overrides the virtual parameterRangeChanged() method inherited from ParameterObserver. */
  virtual void parameterRangeChanged(Parameter* parameterThatHasChanged);

  /** Overrides the purely virtual method of the ParameterObserver base class in order to 
  invalidate our pointer-member 'assignedParameter'. */
  virtual void parameterIsGoingToBeDeleted(Parameter* parameterThatWillBeDeleted);

  /** This method is called when the assigned Parameter has been changed - override it in the 
  subclasses to do the actual GUI update. */
  virtual void updateWidgetFromAssignedParameter(bool sendChangeMessage = false);

  /** Overrides handleAsyncUpdate to call updateWidgetFromAssignedParameter from there. */
  virtual void handleAsyncUpdate(); 

protected:

  /** A pointer to the function which converts a value into a juce::String. */
  String (*stringConversionFunction) (double valueToConvert);
  
  /** The assigned Parameter object (might be a nullptr). */
  Parameter *assignedParameter;


  int highlightLevel;
  int fontSize;



private:

  const Skin *skin; // \todo make this private and force subclasses to use getBackgroundColor(), ...
    // this makes it also possible to have a nullptr as skin in which case these functions may 
    // return default colors and fonts - or, maybe, if in the constructor a nullptr is passed let
    // it point to a default skin which is a static member

  JUCE_LEAK_DETECTOR(RWidget);
};

#endif  
