#ifndef MixsonicEnterProjectNameDialog_h
#define MixsonicEnterProjectNameDialog_h

#include "../widgets/RButton.h"
#include "MixsonicTextEntryDialog.h"

/**

This class implements the dialog to enter the name for a project.

*/

class MixsonicEnterProjectNameDialog : public MixsonicTextEntryDialog, public ButtonListener
{

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  MixsonicEnterProjectNameDialog(SectionSkin *skinToUse, const File& projectParentDirectory);

  /** Destructor. */
  virtual ~MixsonicEnterProjectNameDialog();

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
  RButton *cancelButton, *okButton;

  // the parent directory in which the project should be created:
  File parentDirectory;

  // the name that the user has chosen for the new project:
  String projectName;

};

#endif
