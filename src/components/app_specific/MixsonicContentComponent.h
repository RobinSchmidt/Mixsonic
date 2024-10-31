#ifndef MixsonicContentComponent_h
#define MixsonicContentComponent_h

#include "../../core/infrastructure/StateFileManager.h"
#include "../../core/infrastructure/SamplePool.h"
#include "../../core/audio/AudioRenderingThread.h"
#include "../../core/audio/AudioSampleBufferFunctions.h"
#include "../../control/MixsonicCommandManager.h"
#include "../reusable/ComponentGrabber.h"
#include "../reusable/TransportComponent.h"
#include "MixsonicArrangementEditor.h"
#include "MixsonicSampleBrowser.h"
#include "MixsonicProjectComponent.h"
#include "../widgets/MComboBox.h"
#include "../dialogs/MixsonicAudioExportDialog.h"
#include "../dialogs/MixsonicAudioRecordDialog.h"
#include "../dialogs/MixsonicCreateDirectoryDialog.h" // \todo to be thrown out
#include "../dialogs/MixsonicGlobalSettingsDialog.h"
#include "../dialogs/MixsonicNewProjectDialog.h"
#include "../dialogs/MixsonicEnterProjectNameDialog.h"




class MixsonicMenuBarModel : public MenuBarModel
{
public:
  MixsonicMenuBarModel(ApplicationCommandManager *commandManagerToUse);
  virtual const StringArray getMenuBarNames();
  virtual const PopupMenu getMenuForIndex(int topLevelMenuIndex, const String &menuName);
  virtual void 	menuItemSelected (int menuItemID, int topLevelMenuIndex);
protected:
  ApplicationCommandManager *commandManager;
};



/**

This program implements a very basic and easy to use audio sequencer application.

*/

class MixsonicContentComponent : public Component, public ButtonListener, 
  public ApplicationUserInterface,
  public ChangeListener,public FileBrowserListener,
  public ApplicationCommandTarget, public TransportControlObserver,
  public ChangeBroadcaster, public StateFileManager, public SamplePoolClient
{  

  friend class MixsonicWindow;

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  MixsonicContentComponent(const String &newEditorName, ApplicationSkin *skinToUse);  

  /** Destructor. */
  virtual ~MixsonicContentComponent();              

  //-----------------------------------------------------------------------------------------------
  // inquiry:

  /** Returns the name of the current project. */
  virtual String getProjectName();

  //-----------------------------------------------------------------------------------------------
  // callbacks:

  /** Overrides mouseUp in order to drop clips onto tracks and/or delete any temporary 
  AudioClipComponent. */
  virtual void mouseUp(const MouseEvent &e);

  /** Overrides mouseDrag in order to drag around the clips. */
  virtual void mouseDrag(const MouseEvent &e);

  /** Implements the purely virtual buttonClicked()-method of the ButtonListener base-class. */
  virtual void buttonClicked(Button *buttonThatWasClicked);

  /** Implements the purely virtual method of the ChangeListenerListener base-class.*/
  virtual void changeListenerCallback(ChangeBroadcaster *objectThatHasChanged);

  /** Implements the callback from the FileBrowserListener baseclass. */
  virtual void selectionChanged();
  
  /** Implements the callback from the FileBrowserListener baseclass. */
  virtual void fileClicked(const File &file, const MouseEvent &e);
 
  /** Implements the callback from the FileBrowserListener baseclass. */
  virtual void fileDoubleClicked(const File &file);

  /** Implements the callback from the FileBrowserListener baseclass. */
  virtual void browserRootChanged(const File &newRoot);

  /** Implements the purely virtual method of the SamplePoolClient base class. */
  virtual void samplePathHasChanged(int index, const File& oldFilePath, const File& newFilePath);

  /** Implements the purely virtual method of the SamplePoolClient base class. */
  virtual void sampleIsToBeDeleted(int index);



  // overrides for ApplicationCommandTarget baseclass:
  virtual ApplicationCommandTarget* getNextCommandTarget();
  virtual void getAllCommands(Array<CommandID> &commands);
  virtual void getCommandInfo(CommandID commandID, ApplicationCommandInfo &result);
  virtual bool perform(const InvocationInfo &info);

  // overrides for TransportControlObserver baseclass:
  virtual void playbackStarted();
  virtual void playbackStopped();
  virtual void wasRewindedToStart();



  /** Overrides the setBounds()-method of the Component base-class in order to arrange the 
  widgets according to the size. */
  virtual void resized();

  /** Overrides paint in order to draw the background. */
  virtual void paint(Graphics &g);

  //-----------------------------------------------------------------------------------------------
  // state saving and recall:

  /** Returns the state (i.e. the settings of all relevant parameters) in form of an XmlElement.
  This overrides the purely virtual method of baseclass StateFileManager, which will be used for 
  saving and loading the xml-states to/from files. */
  virtual XmlElement* getStateAsXml() const;

  /** Recalls a state (i.e. the settings of all relevant parameters) from an XmlElement. 
  This overrides the purely virtual method of baseclass StateFileManager, which will be used for 
  saving and loading the xml-states to/from files. */
  virtual void setStateFromXml(const XmlElement& xmlState);

  /** Overrides the method from StateFileManager in order to update the fileNameField in the 
  SongSettingsComponent after loading. */
  virtual bool loadStateFromXmlFile(const File& fileToLoadFrom);

  /** Overrides the method from StateFileManager in order to update the fileNameField in the 
  SongSettingsComponent after saveing. */
  virtual bool saveStateToXmlFile(const File& fileToSaveTo);

  //-----------------------------------------------------------------------------------------------
  // others:

  /** Initializes a new, fresh song with 5 tracks and default settings for all parameters. */
  virtual void initializeSong();

  //===============================================================================================
  juce_UseDebuggingNewOperator;

protected:

  /** Opens a dialog box for exporting the whole mixdown to an audio-file and does the 
  rendering. */
  void showExportDialog();

  /** Opens a dialog box for recording from the input. */
  void showRecordDialog();

  /** Opens a dialog box for letting the user choose a directory/name for a new project. */
  void showNewProjectDialog();

  /** Opens a dialog box for letting the user create a subdirectory in the project's sample 
  directory. */
  void showCreateDirectoryDialog();

  /** Opens a dialog box for letting the user choose global settings such as the project- and 
  sample directories. */
  void showGlobalSettingsDialog();

  /** Creates a temporary project in which the user works when the application was started but not 
  project was loaded yet. Upon the first saving action, the program will the ask for a project 
  name. */
  void createTemporaryProject();

  /** Deletes the temporary project folder from the harddisc. */
  void clearTemporaryProject();

  /** Saves the current project to into the project file - this will also open a dialog for 
  entering a project name if there was not yet assigned a name (i.e. when we are working with a 
  temporary project). */
  void saveProject();

  /** Sets the flag to indicate whether or not the current song file is up to date. */
  void setSongFileUpToDateFlag(bool isUpToDate);

  /** Updates the content of the song file textfield (in the songSettingsComponent). */
  //void updateSongFileField();

  /** Sets up the project directory for the current project. */
  void setProjectDirectory(const File& newProjectDirectory);

  /** Returns either the parent directory of the currently loaded/saved song or some default 
  song-directory, if none is currently loaded/saved. */
  File getProjectDirectory() const;

  /** Tries to create a new project, including the creation of a directory in the 
  projectsParentDirectory and a project file in there and reporst about the success. */
  bool createNewProject(const String& newProjectName, bool newProjectIsTemporary);

  /** Returns true, when the point given by x,y in this component's coordinate system is inside
  the brwoser component, false otherwise. */
  bool isInsideBrowserComponent(int x, int y) const;

  /** Returns true, when the point given by x,y in this component's coordinate system is inside
  the preview component, false otherwise. */
  bool isInsideProjectComponent(int x, int y) const;

  /** Returns true, when the point given by x,y in this component's coordinate system is inside
  the arrangement component, false otherwise. */
  bool isInsideArrangementEditor(int x, int y) const;

  /** Creates a temporary clip component for dragging around. This function creates a new Component
  and assigns it to our member tmpAudioClipComponent - so make sure that 1: the pointer is 
  currently not in use otherwise (indicated by a NULL pointer, which the function will also check 
  for) and 2: make sure to remove (as child-component) and delete the created object somewhere when 
  it's not needed anymore. It will be also assigned to our draggedComponent pointer and prepared
  for dragging around via 
  componentDragger.startDraggingComponent(draggedComponent, &unconstrainedBounds)*/
  //void createTemporaryClipComponentAt(int x, int y);  // juce 1.46
  //void createTemporaryClipComponentAt(const MouseEvent& e); // old
  void createTemporaryClipComponentAt(const MouseEvent& e, const File& f); 

  // these SamplePool and Arrangement objects will be used for the entire lifetime of the 
  // application:
  //SamplePool  *theSamplePool; 
  Arrangement *theArrangement;

  // the subcomponents and some related stuff:
  MixsonicCommandManager     commandManager;
  MixsonicMenuBarModel       menuBarModel;
  MenuBarComponent          *menuBar;
  MixsonicSampleBrowser     *sampleBrowser;
  MixsonicProjectComponent  *projectComponent;
  TransportController        transportController;
  TransportComponent        *transportComponent;
  MixsonicArrangementEditor *arrangementEditor;
  MLabel                    *infoLineLabel, *infoLineTextField;
  // maybe have a toolbar

  // these things are used to create and drag around a temporary clip object when an audio file
  // is clicked or some audio clip is grabbed from the arrangement:
  AudioClipComponent         *draggedComponent;
  ComponentDragger           componentDragger;
  ComponentBoundsConstrainer unconstrainedBounds;
  AudioFileBuffer            tmpAudioFileBuffer;
  AudioClip*                 tmpAudioClip;  

  // this stuff is needed for handling realtime audio playback:
  AudioDeviceManager deviceManager;
  AudioSourcePlayer  audioPlayer;
  MixerAudioSource   masterAudioSource;

  // the TimeSliceThread that will be used for redrawing certain GUI Components:
  TimeSliceThread drawingThread;
    
  // the global application settings:
  //MixsonicSettings settings;

  // some objects to keep track of the current song file, etc:
  //File sampleContentDirectory, projectsParentDirectory;  
  File projectDirectory, projectFile, projectSampleDirectory;
  bool projectFileIsUpToDate, projectIsTemporary;

  // flags to control the screenset:
  bool browserIsVisible;

};

#endif  