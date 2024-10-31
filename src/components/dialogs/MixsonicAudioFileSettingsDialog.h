#ifndef MixsonicAudioFileSettingsDialog_h
#define MixsonicAudioFileSettingsDialog_h

#include "../../core/audio/AudioFileInfo.h"
#include "../../core/audio/AudioRenderingThread.h"
#include "../../core/audio/AudioSampleBufferFunctions.h"
#include "../../core/infrastructure/Arrangement.h"
#include "MixsonicTextEntryDialog.h"
#include "../widgets/MButton.h"
//#include "../widgets/MLabel.h"
#include "../widgets/MComboBox.h"

/**

This class implements a dialog component for setting up settings for an audiofile such as the path 
of the file, samplerate, etc. It serves as baseclass for the MixsonicAudioExportDialog and 
MixsonicAudioRecordDialog and contains their common code.

*/

class MixsonicAudioFileSettingsDialog : public MixsonicTextEntryDialog, public ButtonListener, 
  public ComboBoxListener, public LabelListener
{

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  MixsonicAudioFileSettingsDialog(SectionSkin *skinToUse, 
    const File& defaultTargetFile = File::nonexistent);

  /** Destructor. */
  virtual ~MixsonicAudioFileSettingsDialog();

  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Sets the Label in which the descriptions for the widgets will appear. */
  virtual void setDescriptionField(Label* newDescriptionField);

  //-----------------------------------------------------------------------------------------------
  // callbacks:

  /** Implements the purely virtual buttonClicked()-method of the ButtonListener base-class. */
  virtual void buttonClicked(Button *buttonThatWasClicked);

  /** Implements the purely virtual comboBoxChanged()-method of the ComboBoxListener base-class.*/
  virtual void comboBoxChanged(ComboBox *comboBoxThatHasChanged);

  /** Implements the purely virtual method of the LabelListener base class. */
  virtual void labelTextChanged(Label *labelThatHasChanged);

  /** Overrides resized()-method of the Component base-class in order to arrange the widgets
  according to the size. */
  virtual void resized();

  //===============================================================================================
  juce_UseDebuggingNewOperator;
  
protected:

  /** Updates the AudioFileInfo object (pointed to by member info) according to the current 
  settings of the widgets. */
  void updateAudioFileInfoAccordingToWidgets();

  // an AudioFileInfo object to store the relevant information about the audiofile to be rendered:
  AudioFileInfo fileInfo;

  // the widgets:
  MLabel *targetDirectoryLabel, *targetDirectoryField, *formatLabel, *sampleRateLabel, 
    *bitDepthLabel, *numChannelsLabel;
  MComboBox *formatComboBox, *sampleRateComboBox, *bitDepthComboBox, *numChannelsComboBox;
  MButton   *browseButton, *exitButton;

};

#endif
