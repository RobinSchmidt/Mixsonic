#include "FileManager.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction:

FileManager::FileManager() //: fileFilter(String("*"), String("All Files"))
{
  fileIsUnsaved              = false;
  recurseSubDirectories      = false;
  currentFileName            = String::empty;
  currentFileNameWithStar    = String::empty;
  currentFileFullPath        = String::empty;
  currentFileRelativePath    = String::empty;
  currentDirectory           = getApplicationDirectoryAsString();
  //defaultExtension          = String("defaultExtension");
  defaultExtension           = String::empty;
  wildcardPatterns           = String("*");
  currentFileIndex           = 0;
  numFilesInList             = 0;

  //updateFileList();
}

FileManager::~FileManager()
{

}

//-------------------------------------------------------------------------------------------------
// setup:

bool FileManager::setCurrentDirectory(const String &newDirectory)
{
  // call the overloaded version of this function (for class File):
  return setCurrentDirectory(File(newDirectory));
}

bool FileManager::setCurrentDirectory(const File &newDirectory)
{
  if( newDirectory.exists() && newDirectory.isDirectory() )
  {
    currentDirectory = newDirectory.getFullPathName();
    updateFileList();
    return true;
  }
  else
    return false;
}

void FileManager::incrementCurrentFile()
{
  // make sure that we are not in an empty directory:
  if(numFilesInList <= 0)
    return;

  // increment the index with wraparound:
  currentFileIndex += 1;
  while( currentFileIndex > numFilesInList-1 )
    currentFileIndex -= numFilesInList;

  File fileToLoadFrom = fileList[currentFileIndex];
  if( fileToLoadFrom.existsAsFile() )
  {
    currentFileFullPath     = fileToLoadFrom.getFullPathName();
    currentFileName         = fileToLoadFrom.getFileName();
    currentFileNameWithStar = currentFileName + String("*");
    currentFileRelativePath = fileToLoadFrom.getRelativePathFrom(File::getSpecialLocation(
      File::currentApplicationFile));
  }
  // todo: get rid of the code duplication in decrementCurrentFile
}

void FileManager::decrementCurrentFile()
{
  // make sure that we are not in an empty directory:
  if(numFilesInList <= 0)
    return;

  // increment the index with wraparound:
  currentFileIndex -= 1;
  while( currentFileIndex < 0 )
    currentFileIndex += numFilesInList;

  File fileToLoadFrom = fileList[currentFileIndex];
  if( fileToLoadFrom.existsAsFile() )
  {
    currentFileFullPath     = fileToLoadFrom.getFullPathName();
    currentFileName         = fileToLoadFrom.getFileName();
    currentFileNameWithStar = currentFileName + String("*");
    currentFileRelativePath = fileToLoadFrom.getRelativePathFrom(File::getSpecialLocation(
      File::currentApplicationFile));
  }
}

/*
void FileManager::retrieveCurrentDirectoryFromPreferences(const String &xmlAttributeToLookFor)
{
  // retrieve current application-directory:
  String thisDirectoryAsString = getApplicationDirectoryAsString();

  // look for a 'Preferences.xml' file:
  File preferencesFile = File(thisDirectoryAsString + String("/Preferences.xml") );

  if( preferencesFile.existsAsFile() )
  {
    XmlDocument myDocument(preferencesFile);
    XmlElement* xmlPreferences = myDocument.getDocumentElement();

    if( xmlPreferences != NULL )
    {
      String parentDirectory = xmlPreferences->getStringAttribute(
        String("ContentDirectoryAbsolute"), String("ApplicationDirectory") );

      String subDirectory = xmlPreferences->getStringAttribute(
        xmlAttributeToLookFor, String::empty );

      if( parentDirectory == String("ApplicationDirectory") )
        currentDirectory = thisDirectoryAsString + subDirectory;
      else
        currentDirectory = parentDirectory + subDirectory;

      delete xmlPreferences;
    }
    else
      AlertWindow::showMessageBox(AlertWindow::WarningIcon, String("Alert"), 
      String("Preferences.xml invalid"), String("OK") );
  }
  else
    currentDirectory = thisDirectoryAsString;
}
*/

void FileManager::setRecurseSubDirectories(bool shouldRecurse)
{
  recurseSubDirectories = shouldRecurse;
  updateFileList();
}

//-------------------------------------------------------------------------------------------------
// inquiry:

/*
String FileManager::getApplicationDirectory()
{
  File   thisExeAsFile         = File::getSpecialLocation(File::currentApplicationFile);
  File   thisDirectoryAsFile   = thisExeAsFile.getParentDirectory();
  String thisDirectoryAsString = thisDirectoryAsFile.getFullPathName();
  return thisDirectoryAsString;
}
*/

File FileManager::getCurrentFile() const
{
  return File(currentFileFullPath);
}

File FileManager::getCurrentDirectory() const
{
  return File(currentDirectory);
}

String FileManager::getCurrentDirectoryAsString() const
{
  return currentDirectory;
}

String FileManager::getFileListAsString(const String &fileSeparatorString) const
{
  String fileListString;

  for(int f=0; f<fileList.size(); f++)
    //fileListString += fileList[f]->getFullPathName() + fileSeparatorString; // juce 1.46
    fileListString += fileList[f].getFullPathName() + fileSeparatorString; 

  return fileListString;
}

//-------------------------------------------------------------------------------------------------
// others:

void FileManager::updateFileList()
{
  if(File(currentDirectory).exists() && File(currentDirectory).isDirectory())
  {
    // update the array with the files which are in the current directory (this is needed for 
    // file increment/decrement):
    fileList.clear();

    //File(currentDirectory).findChildFiles(fileList, File::findFiles, false, wildcardPatterns);
    // doesnt work, if we want to match one from a set of different wildcard-pattern, 
    // we have to do it manually:

    // find all files in this directory:
    File(currentDirectory).findChildFiles(fileList, File::findFiles, recurseSubDirectories);

    // remove all files from the array, which do not match the require wildcard-pattern (we can't
    // simply use the wildcardPattern parameter of the findChildFiles-function when we look for
    // several different wildcard-patterns - this works only for one single pattern 
    // (\todo check, if this is still true)):
    int fileIndexToCheck = 0;
    WildcardFileFilter fileFilter(wildcardPatterns, String::empty, 
                                  String("Wildcard filter") );

    while( fileIndexToCheck < fileList.size() )
    {
      // debug:
      //File testFile = fileList[fileIndexToCheck];

      //if( !fileFilter.isFileSuitable(*fileList[fileIndexToCheck]) )  // juce 1.46
      if( !fileFilter.isFileSuitable(fileList[fileIndexToCheck]) ) 
      {
        //fileList.remove(fileIndexToCheck, true);  // juce 1.46
        fileList.remove(fileIndexToCheck);
          // this will also take care of decrementing fileList.size() - that's why we
          // use a while-loop instead of just iterating through all indices of whole array in its
          // initial size
      }
      else
        fileIndexToCheck++;
    }

    numFilesInList = fileList.size();
    fileList.sort(fileComparator);

    // find the index of the currently loaded file inside the current directory:
    currentFileIndex = 0;
    for(int i=0; i<numFilesInList; i++)
    {
      //if(fileList[i]->getFileName() == currentFileName) // juce 1.46
      if(fileList[i].getFileName() == currentFileName)
      {
        currentFileIndex = i;
        break;
      }
    }
  }
}

bool FileManager::openLoadingDialog(const String &dialogTitle)
{
  FileChooser chooser(dialogTitle, File(currentDirectory), wildcardPatterns, true);

  if(chooser.browseForFileToOpen())
  {
    File fileToLoad = chooser.getResult();

    // remember where we are:
    currentFileFullPath     = fileToLoad.getFullPathName();
    currentDirectory        = fileToLoad.getParentDirectory().getFullPathName(); 
    currentFileRelativePath = fileToLoad.getRelativePathFrom(File::getSpecialLocation(
      File::currentApplicationFile));

    // update the preset-field on the GUI
    currentFileName         = fileToLoad.getFileName();
    currentFileNameWithStar = currentFileName + String("*");

    // update the array with the presets which are in the current 
    // preset-directory (which is needed for preset increment/decrement):
    updateFileList();

    return true;
  }
  else
    return false;
}

bool FileManager::openSavingDialog(const String &dialogTitle)
{
  FileChooser chooser(dialogTitle, File(currentDirectory), wildcardPatterns, true);

  if(chooser.browseForFileToSave(true))
  {
    File fileToSaveTo = chooser.getResult();

    // auto append the the extension .xml if the user did not type it in
    // manually:
    if ( !fileToSaveTo.hasFileExtension(defaultExtension) && defaultExtension != String::empty )
      fileToSaveTo = fileToSaveTo.withFileExtension( defaultExtension ) ;

    // remember where we are:
    currentFileFullPath     = fileToSaveTo.getFullPathName();
    currentDirectory        = fileToSaveTo.getParentDirectory().getFullPathName(); 
    currentFileRelativePath = fileToSaveTo.getRelativePathFrom(File::getSpecialLocation(
      File::currentApplicationFile));

    // update the preset-field on the GUI
    currentFileName         = fileToSaveTo.getFileName();
    currentFileNameWithStar = currentFileName + String("*");

    // update the array with the presets which are in the current 
    // preset-directory (which is needed for preset increment/decrement):
    updateFileList();

    return true;
  }
  else
    return false;
}




