#ifndef MixsonicNewProjectDialog_h
#define MixsonicNewProjectDialog_h

#include "../widgets/RButton.h"
#include "MixsonicTextEntryDialog.h"

/**

This class implements the dialog to create a new project.

*/

class MixsonicNewProjectDialog : public MixsonicTextEntryDialog, public ButtonListener
{

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  MixsonicNewProjectDialog(const File& projectParentDirectory);

  /** Destructor. */
  virtual ~MixsonicNewProjectDialog();

  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Sets the Label in which the descriptions for the widgets will appear. */
  virtual void setDescriptionField(Label* newDescriptionField);

  //-----------------------------------------------------------------------------------------------
  // inquiry:

  /** Returns the name that the user has chosen for the new project. */
  const String getChosenNewProjectName();

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

  // the parent directory in which the project should be created:
  File parentDirectory;

  // the directory that the user has chosen for the new project:
  File chosenProjectDirectory;

};

#endif
