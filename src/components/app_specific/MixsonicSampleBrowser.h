#ifndef MixsonicSampleBrowser_h
#define MixsonicSampleBrowser_h

#include "MixsonicBrowserBase.h"
//#include "../reusable/DirectoryTreeComponent.h" // maybe, we can get rid of that class
#include "../reusable/RFileTreeComponent.h"
#include "../widgets/MixsonicSampleBrowserResources.h"

/**

This class implements the filebrowser to be used by the Mixsonic application.

\todo maybe get rid of that MixsonicBrowserBase baseclass
\todo maybe factor out a reusable RFileBrowser baseclass and rename this class into 
      AudioFileBrowser, have an AudioFileBrowserListener class that implements the prelisten
      functionality by responding to a suitable callback, like "audioFileWantsToBePlayed", 
      "audioPlaybackRequest" or something
      maybe handle this by classes AudioPlaybackServer/Client - the client requests playback
      of an AudioBuffer from the server using a callback

*/

class MixsonicSampleBrowser : public MixsonicBrowserBase, public FileBrowserListener
{  

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. Yo must pass a directory to be used as topmost hierachical level for this 
  browser. */
  MixsonicSampleBrowser(SectionSkin *skinToUse, const File &sampleDirectoryToUse);  

  /** Destructor. */
  virtual ~MixsonicSampleBrowser();     

  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Changes the root directory for the browser. */
  virtual void setRootDirectory(const File &newDirectory);  

  /** Adds a FileBrowserListener object that will be called back whenever a new file is selected, 
  etc. inside our audioFileBrowser member.  */
  virtual void addFileBrowserListener(FileBrowserListener *listenerToAdd);

  //-----------------------------------------------------------------------------------------------
  // inquiry:

  /** Returns the currently selected File. */
  virtual const File getSelectedFile() const;

  //-----------------------------------------------------------------------------------------------
  // callbacks:

  /** Implements the callback from the FileBrowserListener baseclass. */
  virtual void selectionChanged();
  
  /** Implements the callback from the FileBrowserListener baseclass. */
  virtual void fileClicked(const File &file, const MouseEvent &e);
 
  /** Implements the callback from the FileBrowserListener baseclass. */
  virtual void fileDoubleClicked(const File &file);

  /** Implements the callback from the FileBrowserListener baseclass. */
  virtual void browserRootChanged(const File &newRoot);

  /** Overrides the setBounds()-method of the Component base-class in order to arrange the 
  widgets according to the size. */
  virtual void resized();

  /** Overrides paint in order to fill the background with the almost white colour. */
  virtual void paint(Graphics &g);

  //===============================================================================================
  juce_UseDebuggingNewOperator;

protected:

  // we use a customized FileTreeComponent for browsing the directories/files:
  DirectoryContentsList *contentsList;
  RFileTreeComponent    *fileTreeComponent;
  FileFilterForBrowser   fileFilter;

};

#endif  