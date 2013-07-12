#ifndef MixsonicGlobals_h
#define MixsonicGlobals_h

//#include "../../libs/JuceIncludes.h"
#include "Skin.h"
#include "MixsonicSettings.h"

#define LOGGING_ENABLED 0 // set this to 1, if you want to write log-messages

/** A class for storing data and objects and providing functions that are accessible 
application-wide, such as search-directories, the skin, a shared mutex-lock, etc. A pointer to an 
object of this class "mixsonicGlobals" is declared here and will be assigned to an object which 
will be created in Mixsonic::initialise and deleted in Mixsonic::shutdown, so it will exist for the 
whole lifetime of the application and be accessible from every file which includes this file.


\todo move some non-GUI related functionality from MixsonicContentComponent to this class.

*/

class MixsonicGlobals : public MixsonicSettingsObserver
{

public:


  MixsonicGlobals();
  ~MixsonicGlobals();




  /** If LOGGING_ENABLED is defined to 1, this function writes a log-message into our log-file. */
  void logMessage(const String& message);



  //-----------------------------------------------------------------------------------------------
  // callbacks:

  virtual void projectsParentDirectoryChanged(const File& newDirectory);
  virtual void sampleContentDirectoryChanged( const File& newDirectory);
  virtual void pluginDirectoriesChanged(      const StringArray& newDirectories);



  /** A mutex-lock object that should be used for synchronizing access to audio-objects such as
  Tracks, Clips, PluginSlots, etc. */
  CriticalSection audioLock;

  /** Our skin, defining the appearance of the graphical user interface. */
  //Skin skin; // is inside the settings

  // settings, like sample-directory, plugin-directories, etc.


  MixsonicSettings settings;


protected:





  KnownPluginList knownPlugins;


  JUCE_LEAK_DETECTOR(MixsonicGlobals);
};

extern MixsonicGlobals *mixsonicGlobals; 




/** Shortcut to mixsonicGlobals->logMessage(). */
void logMessage(const String& message);



#endif
