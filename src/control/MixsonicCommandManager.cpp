#include "MixsonicCommandManager.h"

MixsonicCommandManager::MixsonicCommandManager()
{
  // menu "File":

  ApplicationCommandInfo newProjectInfo(newProjectID);
  newProjectInfo.setInfo("New", "Create a new project", "File", 0);
  newProjectInfo.addDefaultKeypress('n', ModifierKeys::commandModifier);
  registerCommand(newProjectInfo);

  ApplicationCommandInfo openProjectInfo(openProjectID);
  openProjectInfo.setInfo("Open", "Open an existing project", "File", 0);
  openProjectInfo.addDefaultKeypress('o', ModifierKeys::commandModifier);
  registerCommand(openProjectInfo);
  
  ApplicationCommandInfo saveProjectInfo(saveProjectID);
  saveProjectInfo.setInfo("Save", "Save the current project", "File", 0);
  saveProjectInfo.addDefaultKeypress('s', ModifierKeys::commandModifier);
  registerCommand(saveProjectInfo);

  ApplicationCommandInfo renderArrangementInfo(renderArrangementID);
  renderArrangementInfo.setInfo("Render", "Render the arrangement to a file", "File", 0);
  registerCommand(renderArrangementInfo);
    // rename to "Export"

  ApplicationCommandInfo quitInfo(quitID);
  quitInfo.setInfo("Quit", "Quit the application", "File", 0);
  saveProjectInfo.addDefaultKeypress(KeyPress::F4Key, ModifierKeys::altModifier);
  registerCommand(quitInfo);
    // rename to exit


  // menu "Edit":

  ApplicationCommandInfo cutInfo(cutClipsID);
  cutInfo.setInfo("Cut", "Cut selected clips and put them into clipboard", "Edit", 0);
  cutInfo.addDefaultKeypress('x', ModifierKeys::commandModifier);
  registerCommand(cutInfo);

  ApplicationCommandInfo copyInfo(copyClipsID);
  copyInfo.setInfo("Copy", "Copy selected clips into clipboard", "Edit", 0);
  copyInfo.addDefaultKeypress('c', ModifierKeys::commandModifier);
  registerCommand(copyInfo);

  ApplicationCommandInfo pasteInfo(pasteClipsID);
  pasteInfo.setInfo("Paste", "Paste clips from clipboard", "Edit", 0);
  pasteInfo.addDefaultKeypress('v', ModifierKeys::commandModifier);
  registerCommand(pasteInfo);

  // ...insert duplicate...

  ApplicationCommandInfo deleteInfo(deleteClipsID);
  deleteInfo.setInfo("Delete", "Delete selected clips", "Edit", 0);
  deleteInfo.addDefaultKeypress(KeyPress::deleteKey, 0);
  registerCommand(deleteInfo);

  // menu "View":

  ApplicationCommandInfo browserVisibleInfo(toggleBrowserVisibilityID);
  browserVisibleInfo.setInfo("Browser", "Show or hide browser", "View", 0);
  registerCommand(browserVisibleInfo);

  // menu "Setup":

  ApplicationCommandInfo sampleDirectoryInfo(openSampleDirectoryDialogID);
  sampleDirectoryInfo.setInfo("Directories", "Select directories for projects, library, etc.", 
                              "Setup", 0);
  registerCommand(sampleDirectoryInfo);



  // transport controls:





}
