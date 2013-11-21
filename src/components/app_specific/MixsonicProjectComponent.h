#ifndef MixsonicProjectComponent_h
#define MixsonicProjectComponent_h

#include "../../core/infrastructure/SamplePoolClient.h"
#include "MixsonicSampleBrowser.h"
#include "../dialogs/MixsonicEnterNameDialog.h"
#include "../dialogs/MixsonicMoveFileDialog.h"
#include "../reusable/RFileTreeComponent.h"
#include "../reusable/AudioClipComponent.h"
#include "../widgets/MixsonicProjectComponentResources.h"

/**

This class implements the browser for the audiofiles which are already part of the project (i.e.
inside the sample-pool).

\todo rename to SamplePoolBrowser (or similar)

\todo edit the comments, factor out a class ManipulativeFileBrowser (or something) - manipulations
are deletions, movements of files, creation of directories etc.

*/

class MixsonicProjectComponent : public MixsonicSampleBrowser, public SamplePoolClient
{  

  friend class MixsonicContentComponent;

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  MixsonicProjectComponent(SectionSkin *skinToUse, SamplePool *samplePoolToUse, 
    const File& rootDirectory);  

  /** Destructor. */
  virtual ~MixsonicProjectComponent();           

  //-----------------------------------------------------------------------------------------------
  // setup: 

  /** Sets the Label in which the descriptions for the clips will appear. */
  virtual void setDescriptionField(Label* newDescriptionField);

  //-----------------------------------------------------------------------------------------------
  // inquiry:

  /** Returns the (sub)directory which we are currently in. */
  virtual const File getCurrentDirectory() const;
    // \todo maybe move to baseclass

  //-----------------------------------------------------------------------------------------------
  // callbacks:

  /** A callback to indicate that some component was dropped onto this MixsonicArrangementEditor, like, 
  for example, a ClipComponent (or, more precisely, some subclass thereof). The passed coordinates 
  should be with respect to this MixsonicArrangementEditor. The return value informs, whether or not 
  this MixsonicArrangementEditor (or a subcomponent thereof) has taken over the droppedComponent as its 
  own child-component. If true, the caller should NULL all its references to it but MUST NOT delete 
  it - the ArrangementPanel (or some subcomponent inside it) will from now on take care of
  the lifetime of the droppedComponent. If it returns false, it means that the ArrangementPanel 
  was not interested in the droppedComponent - in that case, the caller must take care of it's 
  further lifetime by itself. 
  \todo this comment seems to be wrong(copy/pasted from the ArrangementEditor?)
  */
  virtual void dropComponent(Component* droppedComponent, int x, int y);

  /** Overrides the buttonClicked()-method of the MixsonicBrowserBase base-class. */
  virtual void buttonClicked(Button *buttonThatWasClicked);

  /** Implements the purely virtual method from the SamplePoolClient baseclass in order to adjust 
  the filenames of all clips that use the the buffer which uses the given file. */
  virtual void samplePathHasChanged(int index, const File& oldFilePath, const File& newFilePath);

  /** Implements the purely virtual method from the SamplePoolClient baseclass in order to remove 
  all clips that use the the buffer which uses the given file. */
  virtual void sampleIsToBeDeleted(int index);
  
  /** Implements the callback from the FileBrowserListener baseclass. */
  virtual void fileClicked(const File &file, const MouseEvent &e);

  /** Updates the contents of this component according to the currently loaded project. */
  virtual void updateContents();

  //===============================================================================================
  juce_UseDebuggingNewOperator;

protected:

  /** Shows a context menu which allows to move the file to another folder. */
  bool showFileContextMenu(File fileToMove);

  /** Opens a dialog box for letting the user choose a new name for a file or directory. */
  bool showRenameDialog();

  /** Opens a dialog box for letting the user choose a new directory for a sample file to move 
  to. Returns true when the file was ideed moved, false otherwise. */
  bool showMoveFileDialog();

  /** Attempts to delete the currently selected file from the sample pool, giving the user the 
  chance to abort this action in a warning dialog box. Returns true, if the  file was indeed 
  deleted, false otherwise. */
  bool attemptToDeleteSelectedFile();

  // the additional buttons, specific to the MixsonicProjectComponent:
  MixsonicButton *createDirectoryButton, *eraseSampleButton, *newButton, *openButton, 
    *recordButton, *saveButton;
    // \todo move new, open, save into a menu-bar (into the "File" menu)

};

#endif  