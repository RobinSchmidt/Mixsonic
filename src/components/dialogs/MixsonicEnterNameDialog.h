#ifndef MixsonicEnterNameDialog_h
#define MixsonicEnterNameDialog_h

#include "../widgets/RButton.h"
#include "MixsonicTextEntryDialog.h"

/**

This class implements the dialog to enter a name.

*/

class MixsonicEnterNameDialog : public MixsonicTextEntryDialog, public ButtonListener
{

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  MixsonicEnterNameDialog(const String& defaultName = String::empty);

  /** Destructor. */
  virtual ~MixsonicEnterNameDialog();

  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Sets the Label in which the descriptions for the widgets will appear. */
  virtual void setDescriptionField(Label* newDescriptionField);

  //-----------------------------------------------------------------------------------------------
  // inquiry:

  /** Returns the name that the user has entered. */
  const String getChosenName();

  //-----------------------------------------------------------------------------------------------
  // callbacks:

  /** Implements the purely virtual buttonClicked()-method of the ButtonListener base-class. */
  virtual void buttonClicked(Button *buttonThatWasClicked);

  /** Overrides resized()-method of the Component base-class in order to arrange the widgets
  according to the size. */
  virtual void resized();

  //===============================================================================================
  juce_UseDebuggingNewOperator;
  
protected:

  // the widgets:
  RButton *cancelButton, *okButton;

};

#endif
