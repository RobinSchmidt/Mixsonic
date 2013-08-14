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
  sendProjectsParentDirectoryChangeMessage();
}

void MixsonicSettings::setSampleContentDirectory(const File& newSampleContentDirectory)
{
  sampleContentDirectory = newSampleContentDirectory;
  sendSampleContentDirectoryChangeMessage();
}

void MixsonicSettings::setPluginDirectoriesFromString(const String& directoriesString)
{
  pluginDirectories.clear();
  pluginDirectories.addTokens(directoriesString, String(";"), String::empty);
  sendPluginDirectoriesChangeMessage();
}

//-------------------------------------------------------------------------------------------------
// inquiry:

File MixsonicSettings::getProjectsParentDirectory() const
{
  return projectsParentDirectory;
}

File MixsonicSettings::getSampleContentDirectory() const
{
  return sampleContentDirectory;
}

File MixsonicSettings::getSkinFile() const
{
  return skinFile;
}

StringArray MixsonicSettings::getPluginDirectories() const
{
  return pluginDirectories;
}

//-------------------------------------------------------------------------------------------------
// others:

void MixsonicSettings::initToDefaults()
{
  setSampleContentDirectory( File(getApplicationDirectory()));
  setProjectsParentDirectory(File(getApplicationDirectory()));
  skinFile = File(getApplicationDirectoryAsString() + "/Skins/Default.xml");
  pluginDirectories.clear();
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

      // retrieve the skin-file:
      tmpString = xmlSettings->getStringAttribute("skinFile", String::empty);
      skinFile  = File(getApplicationDirectoryAsString() + "/Skins/" + tmpString);

      // retrieve the plugin directories:
      tmpString = xmlSettings->getStringAttribute("pluginDirectories", String::empty);
      if( tmpString == String::empty )
      {
        showSettingsFileIsInvalidBox();
        delete xmlSettings;
        return false;
      }
      else
        setPluginDirectoriesFromString(tmpString);

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

  xmlState->setAttribute("sampleContentDirectory",  sampleContentDirectory.getFullPathName());
  xmlState->setAttribute("projectsParentDirectory", projectsParentDirectory.getFullPathName());
  xmlState->setAttribute("skinFile", skinFile.getRelativePathFrom(
    File(getApplicationDirectoryAsString() + "/Skins/")));
  xmlState->setAttribute("pluginDirectories",       pluginDirectories.joinIntoString(";"));

  bool success = saveXmlToFile(*xmlState, settingsFile);
  delete xmlState;
  return success;
}


void MixsonicSettings::registerSettingsObserver(MixsonicSettingsObserver *observer)
{
  observers.addIfNotAlreadyThere(observer);
}

void MixsonicSettings::deRegisterSettingsObserver(MixsonicSettingsObserver *observer)
{
  observers.removeValue(observer);
}

void MixsonicSettings::sendProjectsParentDirectoryChangeMessage()
{
  for(int i = 0; i < observers.size(); i++)
    observers[i]->projectsParentDirectoryChanged(projectsParentDirectory);
}

void MixsonicSettings::sendSampleContentDirectoryChangeMessage()
{
  for(int i = 0; i < observers.size(); i++)
    observers[i]->sampleContentDirectoryChanged(sampleContentDirectory);
}

void MixsonicSettings::sendPluginDirectoriesChangeMessage()
{
  for(int i = 0; i < observers.size(); i++)
    observers[i]->pluginDirectoriesChanged(pluginDirectories);
}

void MixsonicSettings::sendAllChangeMessages()
{
  sendProjectsParentDirectoryChangeMessage();
  sendSampleContentDirectoryChangeMessage();
  sendPluginDirectoriesChangeMessage();
}