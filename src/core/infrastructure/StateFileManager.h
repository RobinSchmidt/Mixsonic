#ifndef StateFileManager_h
#define StateFileManager_h

#include "FileManager.h"

/**

This class is intended to be used as base-class for objects that can save and restore their state
from xml-files. Subclasses must implement the functions getStateAsXml and setStateFromXml to store
their state in an XmlElement or to setup themselves according to the data stored in some 
XmlElement. It derives from FileManager in order to facilitate skipping through states based on the
xml-files that live in some directory (like the preset skipping functionality seen in the rs-met 
plugIns).

*/

class StateFileManager : virtual public FileManager
{

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  StateFileManager();  

  /** Destructor. */
  virtual ~StateFileManager();  

  //-----------------------------------------------------------------------------------------------
  // state saving and recall (must be implemented by subclasses):

  /** Must be overriden by subclasses and is intended to return the state of the StateFileManager
  (i.e. the settings of all relevant parameters) in form of an XmlElement. */
  virtual XmlElement* getStateAsXml() const = 0;

  /** Must be overriden by subclasses and is intended to recall the state of the StateFileManager
  (i.e. the settings of all relevant parameters) from an XmlElement. The boolean return value 
  informs, if all required attributes were found in the passed XmlElement. */
  virtual void setStateFromXml(const XmlElement& xmlState) = 0;

  //===============================================================================================
  juce_UseDebuggingNewOperator;

protected:

  /** Overrides the openLoadingDialog() method inherited from FileManager in order to call 
  loadStateFromXmlFile afterwards. */
  virtual bool openLoadingDialog(const String &dialogTitle = String("Load File") );

  /** Overrides the openSavingDialog() method inherited from FileManager in order to call 
  saveStateToXmlFile afterwards. */
  virtual bool openSavingDialog(const String &dialogTitle = String("Save File") );

  /** Tries to load the xml-file passed in the argument and if this was successful, it calls 
  setStateFromXml with the XmlElement obtained from the file. */
  virtual bool loadStateFromXmlFile(const File& fileToLoadFrom);

  /** Retrieves the current state by calling getStateAsXml() and saves this state into an xml-file
  as specified by the argument. */
  virtual bool saveStateToXmlFile(const File& fileToSaveTo);

};

#endif 
