#ifndef MessageBoxes_h
#define MessageBoxes_h

#include "../../libs/JuceIncludes.h"
#include "MixsonicStrings.h"

/** Opens a warning box reporting a memory allocation error. The sourceFunctionName should be the 
name of the function from which the box was summoned. */
void showMemoryAllocationErrorBox(const String& sourceFunctionName);

/** Opens a warning box reporting an invalid audio file - the passed String should be the name of 
the file. */
void showAudioFileInvalidErrorBox(const String& fileName);

/** Opens a warning box reporting a too large audio file */
void showAudioFileTooLargeErrorBox();

/** Shows a box reporting that the audiofile was rendered. */
void showAudioFileRenderedBox(const String& fileName);

/** Opens a warning box reporting that something went wrong when trying to write an audiofile */
void showAudioFileWriteErrorBox(const String& fileName);

/** Shows an error box reporting that an AudioFormatWriter object could not be created. */
void showAudioFormatWriterCreateErrorBox(const String& sourceFunctionName);

/** Opens an error box indicating that some directory could not be created. */
void showDirectoryCouldNotBeCreatedBox(const File& directoryThatCouldNotBeCreated);

/** Opens a warning box reporting that the directory will be removed from the SamplePool and 
deleted from the HD, if the action is continued. */
bool showDirectoryDeleteWarningBox(const String& directoryName);

/** Error-box, indicating that the user has not yet eneterd a name (for a new project). */
void showEnterNameErrorBox();

/** Error-box, indicating that a file could not be copied into an attempted target directory. */
void showFileCouldNotBeCopiedBox(const String& fileName, const String& attemptedDirectoryName);

/** Error-box, indicating that a file could not be deleted. */
void showFileCouldNotBeDeletedBox(const String& fileName);

/** Error-box, indicating that a file could not be moved into an attempted target directory. */
void showFileCouldNotBeMovedBox(const String& fileName, const String& attemptedDirectoryName);

/** Error-box, indicating that a file could not be renamed to the attempted name. */
void showFileCouldNotBeRenamedBox(const String& fileName, const String& attemptedName);

/** Error-box, indicating that a file could not be saved. */
void showFileCouldNotBeSavedBox(const String& fileName);

/** Error-box, indicating that a chosen file should have been a directory but wasn't. */
void showFileIsNoDirectoryErrorBox(const File& file);

/** Error box that the file is either nonexistent or not a valid audiofile. */
void showFileNotFoundOrInvalidAudioFileBox(const String& fileName);

/** Error box that a FileOutputStream object could not be created for the file in question. */
void showFileOutputStreamCreateErrorBox(const File& file);

/** Shows an error box, indicating that a given String does not represent a valid path. */
void showInvalidPathNameErrorBox(const String& theInvalidName);

/** Opens a warning box reporting that the audiofile will be overwritten, if the action is
continued. */
bool showOverwriteAudioFileWarningBox(const String& fileName);

/** Error-box, indicating that the user has not selected a valid project file to load. */
void showPleaseSelectProjectFileBox();

/** Opens an error box indicating that some directory could not be created. */
void showProjectCouldNotBeCreatedBox(const String& projectName);

/** Shows an error box that the project directory that the user wants to create already exists. */
void showProjectAlreadyExistsErrorBox(const String& existingProjectName);

/** Shows an error box that the project parent directory is invalid. \todo: May be later refined to 
a dialog which lets the user choose a new directory. */
void showProjectsParentDirectoryInvalidBox(const String& pathString);

/** Opens a warning box reporting that the songfile will be overwritten, if the action is
continued. */
bool showOverwriteSongFileWarningBox(const String& fileName);

/** Shows a box reporting that audiofile-rendering was aborted by the user. */
void showRenderingAbortedBox();

/** Opens a warning box reporting that the audiofile will be removed from the SamplePool and 
deleted from the HD, if the action is continued. */
bool showSampleDeleteWarningBox(const String& fileName);

/** Shows an error box that the sample content directory is invalid. \todo: May be later refined to 
a dialog which lets the user choose a new directory. */
void showSampleContentPathInvalidBox(const String& pathString);

/** Error-box, indicating that the global application settings file is missing. */
void showSettingsFileIsMissingBox();

/** Error-box, indicating that the global application settings file is invalid. */
void showSettingsFileIsInvalidBox();

/** Opens a warning box reporting that the current song is not saved and will be discarded, if the 
action is continued. */
bool showSongUnsavedWarningBox();

/** Error-box, indicating that a file could not be moved into an attemted target directory because 
in the target directory is already a file with this name. */
void showTargetFileAlreadyExistsBox(const String& fileName, const String& attemptedDirectoryName);

#endif 