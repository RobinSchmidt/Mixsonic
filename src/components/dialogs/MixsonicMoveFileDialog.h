#ifndef MixsonicMoveFileDialog_h
#define MixsonicMoveFileDialog_h

#include "../widgets/RButton.h"
#include "../widgets/RLabel.h"
#include "MixsonicModalDialog.h"
#include "../../core/infrastructure/FileFilters.h"

/**

This class implements the dialog to move a sample file inside the pool.

*/

class MixsonicMoveFileDialog : public MixsonicModalDialog, public ButtonListener, 
  public FileBrowserListener
{

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  MixsonicMoveFileDialog(SectionSkin *skinToUse, const File& navigationRootDirectory, 
    const File& fileToBeMoved);

  /** Destructor. */
  virtual ~MixsonicMoveFileDialog();

  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Sets the Label in which the descriptions for the widgets will appear. */
  virtual void setDescriptionField(Label* newDescriptionField);

  //-----------------------------------------------------------------------------------------------
  // inquiry:

  /** Returns the the directory into which the user wants to move the file. */
  const File getDirectoryToMoveTo();

  //-----------------------------------------------------------------------------------------------
  // callbacks:

  /** Implements the callback from the FileBrowserListener baseclass. */
  virtual void selectionChanged();
  
  /** Implements the callback from the FileBrowserListener baseclass. */
  virtual void fileClicked(const File &file, const MouseEvent &e);
 
  /** Implements the callback from the FileBrowserListener baseclass. */
  virtual void fileDoubleClicked(const File &file);

  /** Implements the callback from the FileBrowserListener baseclass. */
  virtual void browserRootChanged(const File &newRoot); // added after switching to JUCE 2.0

  /** Implements the purely virtual buttonClicked()-method of the ButtonListener base-class. */
  virtual void buttonClicked(Button *buttonThatWasClicked);

  /** Overrides resized()-method of the Component base-class in order to arrange the widgets
  according to the size. */
  virtual void resized();

  //===============================================================================================
  juce_UseDebuggingNewOperator;
  
protected:

  // the FileTreeView and related itemns:
  TimeSliceThread       browserUpdateThread;
  DirectoryContentsList *contentsList;
  FileTreeComponent     *fileTreeComponent;
  DirectoriesOnlyFilter directoriesOnlyFilter;

  // the widgets:
  RButton *cancelButton, *moveButton;

  // the root directory from which the user can navigate:
  File rootDirectory, fileToMove;

  // the directory into which the user wants to move the file:
  File directoryToMoveTo;

};

#endif	
