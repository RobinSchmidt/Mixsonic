#ifndef FileManager_h
#define FileManager_h

#include "GlobalFunctions.h"
#include "FileComparator.h"

/**

This class can be used to manage a bunch of files in a directory to allow for loading, saving,
skipping to next/previous, etc.

*/

class FileManager

{

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  FileManager();  

  /** Destructor. */
  virtual ~FileManager();  

  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Tries to set the current directory from a String and returns whether or not this was 
  successful - if it was, the function also updates the internal file-list. */
  virtual bool setCurrentDirectory(const String &newDirectory);

  /** Tries to set the current directory from a File and returns whether or not this was 
  successful - if it was, the function also updates the internal file-list. */
  virtual bool setCurrentDirectory(const File &newDirectory);
   
  /** Adjusts the currentFile... variables such that they point to the next file in the current 
  directory. The actual loading actions must be implemented by subclasses. */
  virtual void incrementCurrentFile();

  /** Adjusts the currentFile... variables such that they point to the previous file in the current 
  directory. The actual loading actions must be implemented by subclasses. */
  virtual void decrementCurrentFile(); 

  /** Tries to set the currentDirectory to a default value which is expected to be specified by 
  some XML-attribute in a Preferences.xml file which is assumed to be in the application   
  directory. */
  //virtual void retrieveCurrentDirectoryFromPreferences(const String &xmlAttributeToLookFor);

  /** Selects, whether or not subdirectories should be scanned recursively when updating the 
  fileList. */
  virtual void setRecurseSubDirectories(bool shouldRecurse);

  //-----------------------------------------------------------------------------------------------
  // inquiry:

  /** Returns the directory of the current application (or .dll) as String. */
  //static String getApplicationDirectory();

  /** Returns the currently marked file (in order to load it or whatever else. */
  virtual File getCurrentFile() const;

  /** Returns the current directory (as juce::File). */
  virtual File getCurrentDirectory() const;

  /** Returns the current directory (as juce::String). */
  virtual String getCurrentDirectoryAsString() const;

  /** Returns the list of files as a String. */
  virtual String getFileListAsString(const String &fileSeparatorString = String("\n") ) const;

  //-----------------------------------------------------------------------------------------------
  // others:

  /** Updates the array which contains all the relevant files in order to skip through them via 
  plus/minus buttons. */
  virtual void updateFileList();

  /* Adds files from the rootDirectory to the member 'fileList' */
  /*
  virtual void addFilesFromDirectoryToFileList(const File &rootDirectory, 
    bool recurseSubDirectories);*/

  //===============================================================================================
  juce_UseDebuggingNewOperator;

protected:

  /** Opens a dialog-box to load from a file and returns true when a file was loaded and false
  when the dialog was closed without loading anything. */
  virtual bool openLoadingDialog(const String &dialogTitle = String("Load File") );

  /** Opens a dialog-box to save to a fileand returns true when a file was loaded and false
  when the dialog was closed without saving anything. */
  virtual bool openSavingDialog(const String &dialogTitle = String("Save File") );

  // for remebering the directories which was most recently browsed for files:
  bool   fileIsUnsaved;
  bool   recurseSubDirectories;
  String currentFileName;
  String currentFileNameWithStar;
  String currentFileFullPath;
  String currentFileRelativePath;
  String currentDirectory;
  String defaultExtension;
  String wildcardPatterns;
  //WildcardFileFilter fileFilter;

  //OwnedArray<File> fileList; // holds all relevant files in a flat array, juce 1.46
  Array<File> fileList;  // holds all relevant files in a flat array
  int numFilesInList;    // holds the number of files in the array
  int currentFileIndex;  // index of the currently marked file in the array

  /** This object is needed to sort the file-array. */
  FileComparator fileComparator;       

};

#endif  // end of #ifndef FileManager_h