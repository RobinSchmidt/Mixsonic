#ifndef DescribedItem_h
#define DescribedItem_h

#include "../../core/infrastructure/GlobalFunctions.h"

/**

This class serves as base class for various GUI-objects that may have a description which can be 
made to appear in some dedicated Label.

*/

class DescribedItem 
{

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  DescribedItem(const String& newDescription = String("some item"));   

  /** Destructor. */
  virtual ~DescribedItem(); 

  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Sets a description for this widget - this should be short enough to fit into the Label which 
  is assigned to the descriptionField member. @see setDescriptionField() */
  virtual void setDescription(const String &newDescription);

  /** Sets the juce::Label in which the description will appear. */
  virtual void setDescriptionField(Label* newDescriptionField);

  //-----------------------------------------------------------------------------------------------
  // inquiry:

  /** Returns the description for this widget. */
  virtual String getDescription() const;

  /** Returns the description field for this item (or NULL if none). */
  virtual Label* getDescriptionField() const;

  //===============================================================================================
  juce_UseDebuggingNewOperator;

protected:

  /** A description of the parameter. */
  String description;

  /** A label where the description will appear when the mouse is over the parameter. */
  Label* descriptionField;

};

#endif  
