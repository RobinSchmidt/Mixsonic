#ifndef MixsonicSubBrowser_h
#define MixsonicSubBrowser_h

#include "../../core/infrastructure/GlobalFunctions.h"
//#include "../../control/Skin.h"
#include "../reusable/UserInterfaceSection.h"

/**

This class implements a sub-browser to be used either as browser for the audio-library or the pool
of audiofiles in the current project.

*/

class MixsonicSubBrowser : public Component, public FileBrowserListener, public ChangeBroadcaster,
    public UserInterfaceSection
{  

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  MixsonicSubBrowser(const FileFilter *const fileFilter, TimeSliceThread &threadToUse);  

  /** Destructor. */
  virtual ~MixsonicSubBrowser();           

  //-----------------------------------------------------------------------------------------------
  // setup: 

  /** Sets the Label in which the descriptions for the clips will appear. */
  virtual void setDescriptionField(Label* newDescriptionField);

  /** Sets the directory to look in for files. */
  virtual void setDirectory(const File &directory, const bool includeDirectories, 
    const bool includeFiles);

  /** Adds a FileBrowserListener object that will be called back whenever a new file is selected, 
  etc. */
  virtual void addFileBrowserListener(FileBrowserListener *listenerToAdd);

  //-----------------------------------------------------------------------------------------------
  // inquiry:

  /** Returns the currently selected file (or directory). */
  virtual const File getSelectedFile() const;

  //-----------------------------------------------------------------------------------------------
  // callbacks:

  // \todo: maybe move these 4 callbacks to MixsonicBrowserBase:
  /** Implements the callback from the FileBrowserListener baseclass. */
  virtual void selectionChanged();
  
  /** Implements the callback from the FileBrowserListener baseclass. */
  virtual void fileClicked(const File &file, const MouseEvent &e);
 
  /** Implements the callback from the FileBrowserListener baseclass. */
  virtual void fileDoubleClicked(const File &file);

  /** Implements the callback from the FileBrowserListener baseclass. */
  virtual void browserRootChanged(const File &newRoot); // added after switching to JUCE 2.0

  /** Overrides the setBounds()-method of the Component base-class in order to arrange the 
  widgets according to the size. */
  virtual void resized();

  /** Overrides paint in order to fill the background with the almost white colour. */
  virtual void paint(Graphics &g);

  //===============================================================================================
  juce_UseDebuggingNewOperator;

protected:

  DirectoryContentsList *contentsList;
  FileListComponent     *fileListComponent;

};

#endif  