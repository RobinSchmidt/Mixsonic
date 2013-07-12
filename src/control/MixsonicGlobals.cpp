#include "MixsonicGlobals.h"
#include "../components/dialogs/MixsonicPluginScannerBox.h"

MixsonicGlobals *mixsonicGlobals = nullptr;   

MixsonicGlobals::MixsonicGlobals()
{
  if( LOGGING_ENABLED )
    logMessage(String::empty); // as separator between runs of the app
  settings.registerSettingsObserver(this);
  
  // initialize the application settings from the MicsonicSettings.xml file which is supposed to 
  // reside in the application directory:
  settings.loadFromFile();
}  

MixsonicGlobals::~MixsonicGlobals()
{
  settings.deRegisterSettingsObserver(this);
}

void MixsonicGlobals::logMessage(const String& message)
{
  if( LOGGING_ENABLED )
  {
    String logString = Time::getCurrentTime().toString(true, true, true, true) 
      + String(":   ") + message + String("\n"); 
    File logFile(File(getApplicationDirectoryAsString() + String("/MixsonicLog.txt")));
    if( !logFile.existsAsFile() )
      logFile.create();
    logFile.appendText(logString);
  }
}

void MixsonicGlobals::projectsParentDirectoryChanged(const File& newDirectory)
{

}

void MixsonicGlobals::sampleContentDirectoryChanged( const File& newDirectory)
{

}

void MixsonicGlobals::pluginDirectoriesChanged(const StringArray& newDirectories)
{
  knownPlugins.clear();
  VSTPluginFormat vstFormat;
  FileSearchPath path(newDirectories.joinIntoString(";"));


  String crashedPluginsPath = getApplicationDirectoryAsString() + String(File::separatorString)
                        + String("CrashedPlugins.txt");
  File crashedPluginsFile(crashedPluginsPath); 
    // later, specify a file here, where the crashed plugins shall be listed - this is used by the 
    // scanner (the scanner writes and reads to/from this file)

  PluginDirectoryScanner scanner(knownPlugins, vstFormat, path, true, crashedPluginsFile);
  String nextFile;
  do
    nextFile = scanner.getNextPluginFileThatWillBeScanned();
  while( scanner.scanNextFile(true) );

  /*
  // todo: open a message box "Scanning Plugins" with progress bar, like:
  // showPluginScannerBox(PluginDirectoryScanner& scanner), that runs this loop and shows the 
  // progress
  // this doesn't work - the box is never painted:
  MixsonicPluginScannerBox scannerBox;
  scannerBox.addToDesktop(0);
  scannerBox.setOpaque(true);
  scannerBox.setVisible(true);
  scannerBox.centreWithSize(300, 100);
  scannerBox.toFront(false);
  scannerBox.repaint();
  scannerBox.scanPluginsAndShowProgress(scanner);
  scannerBox.removeFromDesktop();
  */
}

void logMessage(const String& message)
{
  jassert( mixsonicGlobals != nullptr ); // the mixsonicGlobals was not yet created
  mixsonicGlobals->logMessage(message);
}

