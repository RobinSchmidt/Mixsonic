#include "MessageBoxes.h"

void showMemoryAllocationErrorBox(const String& sourceFunctionName)
{
	jassertfalse;
  AlertWindow::showMessageBox(AlertWindow::WarningIcon, memAllocErrorStr1, 
    memAllocErrorStr2 + sourceFunctionName, okStr);
}

void showAudioFileInvalidErrorBox(const String& fileName)
{
	jassertfalse;
  AlertWindow::showMessageBox(AlertWindow::WarningIcon, invalidAudioFileStr1, 
    theFileStr + fileName + invalidAudioFileStr2, okStr);
}

void showAudioFileRenderedBox(const String& fileName)
{
  AlertWindow::showMessageBox(AlertWindow::InfoIcon, renderingFinishedStr, 
    theArrangementStr + wasRenderedStr + fileName, okStr);
}

void showAudioFileTooLargeErrorBox()
{
	jassertfalse;
  AlertWindow::showMessageBox(AlertWindow::WarningIcon, audioFileTooLargeStr1, 
    audioFileTooLargeStr2 + String(INT_MAX) + audioFileTooLargeStr3, okStr);
}

void showAudioFileWriteErrorBox(const String& fileName)
{
	jassertfalse;
  return AlertWindow::showMessageBox(AlertWindow::WarningIcon, audioFileWriteErrorStr1, 
    theAudioFileStr + fileName 
    + audioFileWriteErrorStr3, okStr);
}

void showAudioFormatWriterCreateErrorBox(const String& sourceFunctionName)
{
	jassertfalse;
  return AlertWindow::showMessageBox(AlertWindow::WarningIcon, audioFormatWriterCreateErrorStr1, 
    audioFormatWriterCreateErrorStr2 + sourceFunctionName, okStr);
}

void showDirectoryCouldNotBeCreatedBox(const File& directoryThatCouldNotBeCreated)
{
	jassertfalse;
  AlertWindow::showMessageBox(AlertWindow::WarningIcon, dirCreateErrorStr1, 
    theDirectoryStr +  directoryThatCouldNotBeCreated.getFullPathName()
    + creationFailedStr, okStr);
}

bool showDirectoryDeleteWarningBox(const String& directoryName)
{
  return AlertWindow::showOkCancelBox(AlertWindow::WarningIcon, dirDeleteWarningStr1, 
    theDirectoryStr + directoryName + dirDeleteWarningStr2, okStr, cancelStr);
}

void showEnterNameErrorBox()
{
  AlertWindow::showMessageBox(AlertWindow::WarningIcon, enterProjectNameStr1, 
    enterProjectNameStr2, okStr);
}

void showFileCouldNotBeCopiedBox(const String& fileName, const String& attemptedDirectoryName)
{
	jassertfalse;
  return AlertWindow::showMessageBox(AlertWindow::WarningIcon, fileCopyErrorStr1, 
    theFileStr + fileName + fileCopyErrorStr2 + attemptedDirectoryName, okStr);
}

void showFileCouldNotBeDeletedBox(const String& fileName)
{
	jassertfalse;
  return AlertWindow::showMessageBox(AlertWindow::WarningIcon, fileDeleteErrorStr1, 
    theFileStr + fileName + fileDeleteErrorStr2, okStr);
}

void showFileCouldNotBeMovedBox(const String& fileName, const String& attemptedDirectoryName)
{
	jassertfalse;
  return AlertWindow::showMessageBox(AlertWindow::WarningIcon, fileMoveErrorStr1, 
    theFileStr + fileName + fileMoveErrorStr2 + attemptedDirectoryName, okStr);
}

void showFileCouldNotBeRenamedBox(const String& fileName, const String& attemptedName)
{
	jassertfalse;
  return AlertWindow::showMessageBox(AlertWindow::WarningIcon, fileRenameErrorStr1, 
    theFileStr + fileName + fileRenameErrorStr2 + attemptedName, okStr);
}

void showFileCouldNotBeSavedBox(const String& fileName)
{
	jassertfalse;
  return AlertWindow::showMessageBox(AlertWindow::WarningIcon, fileSaveErrorStr1, 
    theFileStr + fileName + fileSaveErrorStr2, okStr);
}

void showFileIsNoDirectoryErrorBox(const File& file)
{
	jassertfalse;
  return AlertWindow::showMessageBox(AlertWindow::WarningIcon, fileIsNoDirStr1, 
    fileIsNoDirStr2 + file.getFileName() + fileIsNoDirStr3, okStr);
}

void showFileNotFoundOrInvalidAudioFileBox(const String& fileName)
{
	jassertfalse;
  return AlertWindow::showMessageBox(AlertWindow::WarningIcon, audioFileInvalidStr1, 
    theFileStr + fileName + audioFileInvalidStr2, okStr);
}

void showFileOutputStreamCreateErrorBox(const File& file)
{
	jassertfalse;
  return AlertWindow::showMessageBox(AlertWindow::WarningIcon, fileOutputStreamCreateErrorStr1, 
    fileOutputStreamCreateErrorStr2 + file.getFullPathName() + fileOutputStreamCreateErrorStr3, 
    okStr);
}

void showInvalidPathNameErrorBox(const String& theInvalidName)
{
  return AlertWindow::showMessageBox(AlertWindow::WarningIcon, invalidPathNameStr1, 
    invalidPathNameStr2 + theInvalidName + invalidPathNameStr3, okStr);
}

bool showOverwriteAudioFileWarningBox(const String& fileName)
{
  return AlertWindow::showOkCancelBox(AlertWindow::WarningIcon, overwriteFileStr, 
    theAudioFileStr + fileName + overwriteRequestStr, okStr, cancelStr);
}

bool showOverwriteSongFileWarningBox(const String& fileName)
{
  return AlertWindow::showOkCancelBox(AlertWindow::WarningIcon, overwriteFileStr, 
    theFileStr + fileName + overwriteRequestStr, okStr, cancelStr);
}

void showPleaseSelectProjectFileBox()
{
  AlertWindow::showMessageBox(AlertWindow::WarningIcon, selectProjectFileStr1, 
    selectProjectFileStr2, okStr);
}

void showProjectCouldNotBeCreatedBox(const String& projectName)
{
	jassertfalse;
  AlertWindow::showMessageBox(AlertWindow::WarningIcon, projectCreationErrorStr1, 
    theProjectStr + projectName + creationFailedStr, okStr);
}

void showProjectAlreadyExistsErrorBox(const String& existingProjectName)
{
  return AlertWindow::showMessageBox(AlertWindow::WarningIcon, projectAlreadyExistsStr1, 
    theProjectStr + existingProjectName + projectAlreadyExistsStr2, okStr);
}

void showProjectsParentDirectoryInvalidBox(const String& pathString)
{
	jassertfalse;
  return AlertWindow::showMessageBox(AlertWindow::WarningIcon, invalidProjectParentPathStr1, 
    invalidProjectParentPathStr2 + pathString + invalidProjectParentPathStr3, okStr);
}

void showRenderingAbortedBox()
{
  AlertWindow::showMessageBox(AlertWindow::InfoIcon, renderingAbortedStr, 
    renderingAbortedStr2, okStr);
}

bool showSampleDeleteWarningBox(const String& fileName)
{
  return AlertWindow::showOkCancelBox(AlertWindow::WarningIcon, audiofileDeleteWarningStr1, 
    theAudioFileStr + fileName + audiofileDeleteWarningStr2, okStr, cancelStr);
}

void showSampleContentPathInvalidBox(const String& pathString)
{
  return AlertWindow::showMessageBox(AlertWindow::WarningIcon, invalidSampleContentPathStr1, 
    thePathStr + pathString + invalidSampleContentPathStr2, okStr);
}

void showSettingsFileIsMissingBox()
{
  AlertWindow::showMessageBox(AlertWindow::WarningIcon, settingsFileMissingStr1, 
    settingsFileMissingStr2, okStr);
}

void showSettingsFileIsInvalidBox()
{
	jassertfalse;
  AlertWindow::showMessageBox(AlertWindow::WarningIcon, settingsFileInvalidStr1, 
    settingsFileInvalidStr2, okStr);
}

bool showSongUnsavedWarningBox()
{
  return AlertWindow::showOkCancelBox(AlertWindow::WarningIcon, songUnsavedStr1, 
    songUnsavedStr2, okStr, cancelStr);
}

void showTargetFileAlreadyExistsBox(const String& fileName, const String& attemptedDirectoryName)
{
  return AlertWindow::showMessageBox(AlertWindow::WarningIcon, 
    fileNotMovedBecauseAlreadyExistsStr1, 
    theFileStr + fileName + fileNotMovedBecauseAlreadyExistsStr2 + attemptedDirectoryName 
    + fileNotMovedBecauseAlreadyExistsStr3, okStr );
}
