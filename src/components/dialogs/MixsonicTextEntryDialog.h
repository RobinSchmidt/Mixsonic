#ifndef MixsonicTextEntryDialog_h
#define MixsonicTextEntryDialog_h

#include "../widgets/RLabel.h"
#include "../widgets/MixsonicTextEntryField.h"
#include "MixsonicModalDialog.h"

/**

This class is to be used as baseclass for dialogs which have a text entry field.

*/

class MixsonicTextEntryDialog : public MixsonicModalDialog
{

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  MixsonicTextEntryDialog(SectionSkin *skinToUse, const String& labelText, 
    const String& initialText = String::empty);

  /** Destructor. */
  virtual ~MixsonicTextEntryDialog();

  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Sets the text for the text entry field. */
  virtual void setText(const String& newText);

  /** Sets the text that appears in front of the actual text entry field. */
  virtual void setLabelText(const String& newText);

  /** Passes the description to both - the actual entry field and its label. */
  virtual void setDescription(const String& newDescription);

  /** Sets the Label in which the descriptions for the widgets will appear. */
  virtual void setDescriptionField(Label* newDescriptionField);

  //-----------------------------------------------------------------------------------------------
  // callbacks:

  /** Overrides grabKeyboardFocus() in order to pass the focus to our child component - the actual 
  MixsonicTextEntryField. */
  virtual void grabKeyboardFocus();

  //===============================================================================================
  juce_UseDebuggingNewOperator;
  
protected:

  // the widgets:
  RLabel                 *textEntryLabel;
  MixsonicTextEntryField *textEntryField;

};

#endif
