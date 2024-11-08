#ifndef MixsonicGlobalSettingsDialog_h
#define MixsonicGlobalSettingsDialog_h

#include "../widgets/MLabel.h"
#include "../widgets/MButton.h"
#include "../widgets/MixsonicTextEntryField.h"
#include "../../control/MixsonicSettings.h"
#include "MixsonicModalDialog.h"

/**

This class implements the dialog for setting up some global settings like the project- and 
content-directories.

*/

class MixsonicGlobalSettingsDialog : public MixsonicModalDialog, public ButtonListener
{

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. You must pass a pointer to a MixsonicSettings object - this object will be 
  manipulated by the dialog. */
  MixsonicGlobalSettingsDialog(SectionSkin *skinToUse, MixsonicSettings* settingsToManipulate);

  /** Destructor. */
  virtual ~MixsonicGlobalSettingsDialog();

  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Overrides setDescriptionField(). */
  virtual void setDescriptionField(Label* newDescriptionField);

  //-----------------------------------------------------------------------------------------------
  // callbacks:

  /** Overrides buttonClicked(). */
  virtual void buttonClicked(Button *buttonThatWasClicked);

  /** Overrides resized(). */
  virtual void resized();

  //===============================================================================================
  juce_UseDebuggingNewOperator;
  
protected:

  // the widgets specific to this dialog:
  MLabel                 *projectsParentDirectoryLabel,        *sampleContentDirectoryLabel;;
  MixsonicTextEntryField *projectsParentDirectoryTextField,    *sampleContentDirectoryTextField;

  //FileSearchPathListComponent *pluginDirectoriesList;
  //RListBox pluginDirectoriesList; // we need to write this class

  MButton                *projectsParentDirectoryBrowseButton, *sampleContentDirectoryBrowseButton,
                         *cancelButton, *okButton;

  MixsonicSettings *settings;      // the actual app-settings to be manipulated
  MixsonicSettings  settingsTmp;   // temporary settings (to allow for cancelling the changes)

};

#endif	
