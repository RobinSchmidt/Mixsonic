#ifndef MixsonicCommandManager_h
#define MixsonicCommandManager_h

#include "../../libs/JuceIncludes.h"

class MixsonicCommandManager : public ApplicationCommandManager
{

public:

  enum commandIDs
  {
    // menu "File":  ...or maybe call it "Project"
    newProjectID = 1,  // 0 isn't a valid ID
    openProjectID,
    saveProjectID,
    //saveProjectAsID,
    //projectSettingsID, // timeFormat, tempo, sampleRate, etc.
    renderArrangementID, // rename to exportSongID
    //exportSelectedClipsID
    //exportSelectedTracksID
    quitID,

    // menu "Edit":
    cutClipsID,
    copyClipsID,
    pasteClipsID,
    //pasteClipsAsAliasID,
    duplicateClipsID,
    //duplicateClipsAsAliasID
    deleteClipsID,
    splitClipsID,
    //mergeClipsID, // maybe use a composite pattern for clips, merging audio-clips would then
                    // amount to put them into a container-clip, MIDI clips could be merged 
                    // more directly
    //undoID,
    //redoID,
    //invertClipSelectionID,
    //createTrackID,
    //duplicateTrackID,
    //createClipID,


    // menu "View":
    toggleBrowserVisibilityID,
    //toggleInputComponentVisibilityID,
    //toggleOutputComponentVisibilityID,
    //toggleClipEditorVisibilityID,
    //switchToArrangeViewID,
    //switchToMixerViewID,

    // menu "Setup":
    openSampleDirectoryDialogID,
    //openPluginDirectoryDialogID,
    //openAudioSetupID,
    //openColorSchemeSetupID,


    /*
    // nah - we don't implement them as app-commands
    // transport buttons:
    togglePlayID,
    rewindToStartID,
    skipMarkerBackID,
    skipMarkerForwardID,
    skipMeasureBackID,
    skipMeasureForwardID,
    */

    numCommandIDsPlus1
  };

  /** Constructor.  */
  MixsonicCommandManager::MixsonicCommandManager();

};

#endif