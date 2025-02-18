#include "RPopUpMenu.h"


RPopUpMenu::RPopUpMenu(Component *componentToAttachTo) 
  : ROwnedPopUpComponent(componentToAttachTo, nullptr) // preliminary - pass a skin as 2nd parameter
{
  rootNode = new RTreeViewNode();
  rootNode->setDeleteChildNodesOnDestruction(true);

  treeView = new RTreeLeafNodeSelector();
  treeView->setRootNode(rootNode);
  treeView->setDrawRootNode(false);
  //treeView->setDrawRootNode(true);
  treeView->registerTreeViewObserver(this);
  setContentWidget(treeView);
  // the RTreeView::paint function is not called - instead, it calls RWidget::paint

  setAlwaysOnTop(true);
  setOpaque(true);
}

RPopUpMenu::~RPopUpMenu()
{
  treeView->deRegisterTreeViewObserver(this);
  treeView->setRootNode(NULL);
  delete rootNode;
}

void RPopUpMenu::addItem(int itemResultId, const juce::String& itemText, bool isEnabled, bool isTicked)
{
  addTreeNodeItem( new RTreeViewNode(itemText, itemResultId,
    String::empty, // preliminary - add item description here later
    isEnabled, isTicked) );

  /*
  rootNode->addChildNode( new RTreeViewNode(itemText, itemResultId,
    String::empty, // preliminary - add item description here later
    isEnabled, isTicked) );
    */
}

void RPopUpMenu::addTreeNodeItem(RTreeViewNode *nodeToAdd)
{
  rootNode->addChildNode(nodeToAdd);
}

void RPopUpMenu::clear()
{
  treeView->deSelectNode();
  rootNode->deleteChildNodesRecursively();
}
   
void RPopUpMenu::setItemText(int index, const juce::String& newText)
{
  if( index >= 0 && index < getNumItems() )
    getItemByIndex(index)->setNodeText(newText);
  repaint();
}

void RPopUpMenu::setItemEnabled(int index, bool shouldBeEnabled)
{
  if( index >= 0 && index < getNumItems() )
    getItemByIndex(index)->setEnabled(shouldBeEnabled);
  repaint();
}

void RPopUpMenu::selectItemByIndex(int itemIndex, bool sendNotification)
{
  treeView->selectNodeByIndex(itemIndex, false);
  if( sendNotification == true )
    sendPopUpSelectionNotification();
}

void RPopUpMenu::selectItemByIdentifier(int itemIdentifier, bool sendNotification)
{
  treeView->selectNodeByIdentifier(itemIdentifier, false);
  if( sendNotification == true )
    sendPopUpSelectionNotification();
}

void RPopUpMenu::registerPopUpMenuObserver(RPopUpMenuObserver* const observerToRegister)
{
  popUpMenuObservers.addIfNotAlreadyThere(observerToRegister);
}

void RPopUpMenu::deRegisterPopUpMenuObserver(RPopUpMenuObserver* const observerToDeRegister)
{
  popUpMenuObservers.removeValue(observerToDeRegister);
}

RTreeViewNode* RPopUpMenu::getItemByIndex(int index) const
{
  if( index < 0 || index >= treeView->rootNode->childNodes.size() || treeView->rootNode == NULL )
  {
    jassertfalse;  // index out of range
    return NULL;
  }
  else
    return treeView->rootNode->childNodes[index];
}

RTreeViewNode* RPopUpMenu::getSelectedItem() const
{
  return treeView->getSelectedNode();
}

void RPopUpMenu::treeNodeClicked(RTreeView *treeView, RTreeViewNode *nodeThatWasClicked, const MouseEvent &mouseEvent, int clickPosition)
{
  if( nodeThatWasClicked->isLeafNode() )
  {
    dismiss();
    sendPopUpSelectionNotification();
  }
}

void RPopUpMenu::treeNodeChanged(RTreeView *treeView, RTreeViewNode *nodeThatHasChanged)
{
  sendPopUpSelectionNotification();
}

void RPopUpMenu::resized()
{
  ROwnedPopUpComponent::resized();
  treeView->setBounds(0, 0, getWidth(), getHeight());
}

/*
void RPopUpMenu::paint(Graphics &g)
{
  g.fillAll(Colours::blue);
}
*/

/*
void RPopUpMenu::focusLost(FocusChangeType cause)
{
  int dummy = 0;
}
*/

void RPopUpMenu::sendPopUpSelectionNotification()
{
  for(int i=0; i<popUpMenuObservers.size(); i++)
    popUpMenuObservers[i]->rPopUpMenuChanged(this);
}
