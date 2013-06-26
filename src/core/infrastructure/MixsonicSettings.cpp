#include "MixsonicSettings.h"

MixsonicSettings::MixsonicSettings()
{
  initToDefaults();
}

MixsonicSettings::~MixsonicSettings()
{

}

//-------------------------------------------------------------------------------------------------
// setup:

void MixsonicSettings::setProjectsParentDirectory(const File& newProjectsParentDirectory)
{
  projectsParentDirectory = newProjectsParentDirectory;
}

void MixsonicSettings::setSampleContentDirectory(const File& newSampleContentDirectory)
{
  sampleContentDirectory = newSampleContentDirectory;
}

//-------------------------------------------------------------------------------------------------
// inquiry:

File MixsonicSettings::getProjectsParentDirectory()
{
  return projectsParentDirectory;
}

File MixsonicSettings::getSampleContentDirectory()
{
  return sampleContentDirectory;
}

//-------------------------------------------------------------------------------------------------
// others:

void MixsonicSettings::initToDefaults()
{
  setSampleContentDirectory( File(getApplicationDirectory()));
  setProjectsParentDirectory(File(getApplicationDirectory()));
}

bool MixsonicSettings::loadFromFile(const File &fileToLoadFrom)
{
  File settingsFile = fileToLoadFrom;
  if( settingsFile == File::nonexistent )
  {
    settingsFile = File(getApplicationDirectoryAsString() + String(File::separatorString)
                        + String("MixsonicSettings.xml"));
  }

  if( !settingsFile.existsAsFile() )
  {
    showSettingsFileIsMissingBox();
    return false;
  }
  else
  {
    XmlElement* xmlSettings = loadXmlFromFile(settingsFile);
    if( xmlSettings != NULL )
    {
      String tmpString;
      File   tmpFile;

      // retrieve the sample-content directory:
      tmpString = xmlSettings->getStringAttribute("sampleContentDirectory", String::empty);
      if( tmpString == String::empty )
      {
        showSettingsFileIsInvalidBox();
        delete xmlSettings;
        return false;
      }
      else
      {
        tmpFile = File(tmpString);
        if( !tmpFile.exists() || !tmpFile.isDirectory() )
          showSampleContentPathInvalidBox(tmpString);
        else
          setSampleContentDirectory(tmpFile);
      }

      // retrieve the parent directory for the projects:
      tmpString = xmlSettings->getStringAttribute("projectsParentDirectory", String::empty);
      if( tmpString == String::empty )
      {
        showSettingsFileIsInvalidBox();
        delete xmlSettings;
        return false;
      }
      else
      {
        tmpFile = File(tmpString);
        if( !tmpFile.exists() || !tmpFile.isDirectory() )
          showProjectsParentDirectoryInvalidBox(tmpString);
        else
          setProjectsParentDirectory(tmpFile);
      }
      // \todo: maybe we can get rid of some code duplication here. maybe we should use a generic 
      // message box which takes the string as parameter

      delete xmlSettings;
    }
    else  // xmlSettings == 0
    {
      showSettingsFileIsInvalidBox();
      return false;
    }
  }

  return true;
}

bool MixsonicSettings::saveToFile(const File &fileToSaveTo)
{
  File settingsFile = fileToSaveTo;
  if( settingsFile == File::nonexistent )
  {
    settingsFile = File(getApplicationDirectoryAsString() + String(File::separatorString)
                        + String("MixsonicSettings.xml"));
  }

  XmlElement* xmlState = new XmlElement(String("MIXSONICSETTINGS"));

  xmlState->setAttribute("sampleContentDirectory", sampleContentDirectory.getFullPathName() );
  xmlState->setAttribute("projectsParentDirectory", projectsParentDirectory.getFullPathName() );

  bool success = saveXmlToFile(*xmlState, settingsFile);
  delete xmlState;
  return success;
}

