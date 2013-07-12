#ifndef MixsonicSettings_h
#define MixsonicSettings_h

#include "../core/infrastructure/GlobalFunctions.h"
#include "Skin.h"

class MixsonicSettingsObserver
{
public:
  virtual void projectsParentDirectoryChanged(const File& newDirectory) = 0;
  virtual void sampleContentDirectoryChanged( const File& newDirectory) = 0;
  virtual void pluginDirectoriesChanged(      const StringArray& newDirectories) = 0;
  //virtual void skinChanged(const Skin& newSkin) = 0;
};


/**

This class is used to store the general application settings for Mixsonic such as the project- 
and sample directories.

*/

class MixsonicSettings 
{

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. Initializes the settings with default values. */
  MixsonicSettings();

  /** Destructor. */
  ~MixsonicSettings();

  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Sets the directory in which the projects will be saved. */
  void setProjectsParentDirectory(const File& newProjectsParentDirectory);

  /** Sets the root directory for the sample-conetent. */
  void setSampleContentDirectory(const File& newSampleContentDirectory);

  /** Sets up the pluginDirectories array from a string which contains all plugin directories 
  delimited by a semicolon. */
  void setPluginDirectoriesFromString(const String& directoriesString);

  //-----------------------------------------------------------------------------------------------
  // inquiry:

  /** Returns the directory in which the projects will be saved. */
  File getProjectsParentDirectory();

  /** Returns the root directory for the sample-content. */
  File getSampleContentDirectory();

  /** Returns an array with the plugin directories. */
  StringArray getPluginDirectories();

  //-----------------------------------------------------------------------------------------------
  // others:

  /** Initializes all settings to their default values. */
  void initToDefaults();

  /** Tries to retrieve the settings from the given file and reports whether or not this was 
  successful. When File::nonexistent is passed as argument (as in the default argument), the 
  function will look for the default file-location 'MixsonicSettings.xml' in the application 
  directory. */
  bool loadFromFile(const File& fileToLoadFrom = File::nonexistent);

  /** Tries to save the settings into the given file and reports whether or not this was 
  successful (it may fail due to the user not having write permission). When File::nonexistent is 
  passed as argument (as in the default argument), the function will use the default file-location 
  'MixsonicSettings.xml' in the application directory. */
  bool saveToFile(const File& fileToSaveTo = File::nonexistent);

  
  void registerSettingsObserver(MixsonicSettingsObserver *observer);

  void deRegisterSettingsObserver(MixsonicSettingsObserver *observer);


protected:

  void sendProjectsParentDirectoryChangeMessage();
  void sendSampleContentDirectoryChangeMessage();
  void sendPluginDirectoriesChangeMessage();
  void sendAllChangeMessages();


  File projectsParentDirectory, sampleContentDirectory;
  StringArray pluginDirectories;
  //Skin skin; // not yet used

  Array<MixsonicSettingsObserver*> observers;

  JUCE_LEAK_DETECTOR(MixsonicSettings);
};

#endif
