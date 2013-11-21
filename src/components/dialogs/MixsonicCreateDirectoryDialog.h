#ifndef MixsonicCreateDirectoryDialog_h
#define MixsonicCreateDirectoryDialog_h

#include "../widgets/RButton.h"
#include "MixsonicTextEntryDialog.h"

/**

This class implements the dialog to create a new directory inside the project's sample directory.

*/

class MixsonicCreateDirectoryDialog : public MixsonicTextEntryDialog, public ButtonListener
{

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  MixsonicCreateDirectoryDialog(SectionSkin *skinToUse, const File& parentDirectory);

  /** Destructor. */
  virtual ~MixsonicCreateDirectoryDialog();

  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Sets the Label in which the descriptions for the widgets will appear. */
  virtual void setDescriptionField(Label* newDescriptionField);

  //-----------------------------------------------------------------------------------------------
  // inquiry:

  /** Returns the directory that the user has chosen to create. */
  const File getDirectoryToCreate();

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
  RButton *cancelButton, *createButton;

  // the parent directory in which the directory should be created:
  File parentDirectory;

  // the directory that the user wants to create:
  File directoryToCreate;

};

#endif
