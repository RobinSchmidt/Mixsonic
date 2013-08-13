#include "MixsonicMoveFileDialog.h"

MixsonicMoveFileDialog::MixsonicMoveFileDialog(const File& navigationRootDirectory, 
                                               const File& fileToBeMoved)
: browserUpdateThread(String("MoveDialogBrowserUpdateThread"))
{
  setHeadline(moveFileHeadlineStr);

  rootDirectory     = navigationRootDirectory;
  fileToMove        = fileToBeMoved;
  directoryToMoveTo = rootDirectory;

  addAndMakeVisible( moveButton = new RButton(moveButtonStr) );
  moveButton->setDescription(moveButtonHelpStr);
  moveButton->setClickingTogglesState(false);
  moveButton->addListener(this);

  addAndMakeVisible( cancelButton = new RButton(cancelStr) );
  cancelButton->setDescription(cancelStr);
  cancelButton->setClickingTogglesState(false);
  cancelButton->addListener(this);

  contentsList = new DirectoryContentsList(&directoriesOnlyFilter, browserUpdateThread);
  contentsList->setDirectory(rootDirectory, true, false);
  contentsList->refresh();

  addAndMakeVisible( fileTreeComponent = new FileTreeComponent(*contentsList) );
  fileTreeComponent->setColour(FileTreeComponent::backgroundColourId, 
                               Skin::getInstance()->backgroundColor);
  fileTreeComponent->setColour(FileTreeComponent::linesColourId, 
                               Skin::getInstance()->outlineColor);
  fileTreeComponent->setDefaultOpenness(true);
  fileTreeComponent->addListener(this);

  browserUpdateThread.startThread (4);

  setSize(400, 400);
}

MixsonicMoveFileDialog::~MixsonicMoveFileDialog()
{
  browserUpdateThread.stopThread (10000);
  deleteAllChildren();
  delete contentsList;
}

//-------------------------------------------------------------------------------------------------
// setup:

void MixsonicMoveFileDialog::setDescriptionField(Label* newDescriptionField)
{
  moveButton->setDescriptionField(newDescriptionField);
  cancelButton->setDescriptionField(newDescriptionField);
}

//-------------------------------------------------------------------------------------------------
// inquiry

const File MixsonicMoveFileDialog::getDirectoryToMoveTo()
{
  return directoryToMoveTo;
}

//-------------------------------------------------------------------------------------------------
// callbacks:

void MixsonicMoveFileDialog::selectionChanged()
{

}

void MixsonicMoveFileDialog::fileClicked(const File &file, const MouseEvent &e)
{

}

void MixsonicMoveFileDialog::fileDoubleClicked(const File &file)
{

}

void MixsonicMoveFileDialog::browserRootChanged(const File &newRoot)
{

}

void MixsonicMoveFileDialog::buttonClicked(Button *buttonThatWasClicked)
{
  if( buttonThatWasClicked == moveButton )
  {
    directoryToMoveTo = fileTreeComponent->getSelectedFile();
    if( directoryToMoveTo == File::nonexistent )
      directoryToMoveTo = fileToMove.getParentDirectory();;

    exitModalState(1);
  }
  else if( buttonThatWasClicked == cancelButton )
    exitModalState(0);
}

void MixsonicMoveFileDialog::resized()
{
  moveButton->setBounds(getWidth()-72-6, getHeight()-26, 72, 20);  
  cancelButton->setBounds(moveButton->getX()-64-4, getHeight()-26, 64, 20); 
  fileTreeComponent->setBounds(4, 4, getWidth()-8, moveButton->getY()-8);
}




