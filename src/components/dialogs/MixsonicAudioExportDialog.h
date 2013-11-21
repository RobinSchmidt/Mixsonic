#ifndef MixsonicAudioExportDialog_h
#define MixsonicAudioExportDialog_h


#include "MixsonicAudioFileSettingsDialog.h"

/**

This class implements the dialog for exporting an Arrangement to an audiofile.

*/

class MixsonicAudioExportDialog : public MixsonicAudioFileSettingsDialog
{

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  MixsonicAudioExportDialog(SectionSkin *skinToUse, Arrangement *arrangementToExport, 
    const File& defaultTargetFile = File::nonexistent);

  /** Destructor. */
  virtual ~MixsonicAudioExportDialog();

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

  /** Starts the rendering process. */
  void renderNow();

  // the widgets specific to this dialog:
  RButton *renderButton;

  /** Pointer to the Arrangement object which shall be exported. */
  Arrangement *theArrangement;

};

#endif	
