#ifndef MixsonicSettings_h
#define MixsonicSettings_h

#include "GlobalFunctions.h"

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
  virtual ~MixsonicSettings();

  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Sets the directory in which the projects will be saved. */
  virtual void setProjectsParentDirectory(const File& newProjectsParentDirectory);

  /** Sets the root directory for the sample-conetent. */
  virtual void setSampleContentDirectory(const File& newSampleContentDirectory);

  //-----------------------------------------------------------------------------------------------
  // inquiry:

  /** Returns the directory in which the projects will be saved. */
  virtual File getProjectsParentDirectory();

  /** Returns the root directory for the sample-conetent. */
  virtual File getSampleContentDirectory();

  //-----------------------------------------------------------------------------------------------
  // others:

  /** Initializes all settings to their default values. */
  virtual void initToDefaults();

  /** Tries to retrieve the settings from the given file and reports whether or not this was 
  successful. When File::nonexistent is passed as argument (as in the default argument), the 
  function will look for the default file-location 'MixsonicSettings.xml' in the application 
  directory. */
  virtual bool loadFromFile(const File& fileToLoadFrom = File::nonexistent);

  /** Tries to save the settings into the given file and reports whether or not this was 
  successful (it may fail due to the user not having write permission). When File::nonexistent is 
  passed as argument (as in the default argument), the function will use the default file-location 
  'MixsonicSettings.xml' in the application directory. */
  virtual bool saveToFile(const File& fileToSaveTo = File::nonexistent);

  //===============================================================================================
  juce_UseDebuggingNewOperator;
  
protected:

  File projectsParentDirectory, sampleContentDirectory;

};

#endif
