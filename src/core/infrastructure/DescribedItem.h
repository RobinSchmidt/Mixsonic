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

protected:

  /** A description of the parameter. */
  String description;

  /** A label where the description will appear when the mouse is over the parameter. */
  Label* descriptionField;

  JUCE_LEAK_DETECTOR(DescribedItem);

};

//=================================================================================================

/**

Subclass of DescribedItem to be used as baseclass for GUI-objects which are juce::MouseListeners

*/

class DescribedMouseListener : public DescribedItem, virtual public MouseListener
{

public:

  /** Constructor. */
  DescribedMouseListener(const String& newDescription = String("No description available"));

  /** Overrides the mouseEnter callback in order to show the description in the dedicated field 
  when the mouse enters the widget. */
  virtual void mouseEnter(const MouseEvent &e);

  /** Overrides the mouseExit callback in order to make the description disappear when the mouse 
  leaves the widget. */
  virtual void mouseExit(const MouseEvent &e);

  JUCE_LEAK_DETECTOR(DescribedMouseListener);

};

#endif  
