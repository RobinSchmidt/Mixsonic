#ifndef ApplicationGlobals_h
#define ApplicationGlobals_h

//#include "../../libs/JuceIncludes.h"
#include "Skin.h"

/** A class for storing data and objects and providing functions that are accessible 
application-wide, such as search-directories, the skin, a shared mutex-lock, etc. */

class ApplicationGlobals
{

public:

  /** A mutex-lock object that should be used for synchronizing access to audio-objects such as
  Tracks, Clips, PluginSlots, etc. */
  CriticalSection audioLock;

  /** Our skin, defining the appearance of the graphical user interface. */
  Skin skin;

  // settings, like sample-directory, plugin-directories, etc.

protected:




  JUCE_LEAK_DETECTOR(ApplicationGlobals);
};



#endif
