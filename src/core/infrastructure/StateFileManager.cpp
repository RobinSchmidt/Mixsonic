#include "StateFileManager.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction:

StateFileManager::StateFileManager() 
{
  wildcardPatterns = String("*.xml");
  defaultExtension = String(".xml");
}

StateFileManager::~StateFileManager()
{

}

//-------------------------------------------------------------------------------------------------
// others:

bool StateFileManager::openLoadingDialog(const String &dialogTitle)
{
  bool result = FileManager::openLoadingDialog();

  if( result == true )
  {
    // the inherited member currentFileFullPath has now be assigned to the file which should be 
    // loaded - so create a file from the String and do it:
    return loadStateFromXmlFile(File(currentFileFullPath));
  }
  else
    return false;
}

bool StateFileManager::openSavingDialog(const String &dialogTitle)
{
  bool result = FileManager::openSavingDialog();

  if( result == true )
  {
    // the inherited member currentFileFullPath has now be assigned to the file which should be 
    // saved - so create a file from the String and do it:
    return saveStateToXmlFile(File(currentFileFullPath));
  }
  else
    return false;
}

bool StateFileManager::loadStateFromXmlFile(const File& fileToLoadFrom)
{
  if( fileToLoadFrom.existsAsFile() )
  {
    XmlDocument myDocument(fileToLoadFrom);
    XmlElement* xmlState = myDocument.getDocumentElement();

    if( xmlState != NULL )
    {     
      setStateFromXml(*xmlState);
      updateFileList();
      delete xmlState;
      return true;
    }
    else
      return false;
  }
  else
    return false;
}

bool StateFileManager::saveStateToXmlFile(const File& fileToSaveTo)
{
  XmlElement* xmlState = getStateAsXml();
  bool success         = saveXmlToFile(*xmlState, fileToSaveTo);
  delete xmlState;
  return success;
}







