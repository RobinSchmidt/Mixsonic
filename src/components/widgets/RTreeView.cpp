#include "RTreeView.h"

RTreeViewNode::RTreeViewNode(const juce::String& _nodeText, int _identifier, const juce::String& _description, 
                             bool _isEnabled, bool _isTicked, bool _isOpen, RTreeViewNode *_parentNode)
{
  this->identifier  = _identifier;
  this->nodeText    = _nodeText;
  this->description = _description;
  this->isEnabled   = _isEnabled;
  this->isTicked    = _isTicked;
  this->isOpen      = _isOpen;
  this->parentNode  = _parentNode;
  this->userData    = NULL;
  deleteChildNodesOnDestruction = false;
}

RTreeViewNode::~RTreeViewNode()
{
  if( deleteChildNodesOnDestruction == true )
    deleteChildNodesRecursively();
}

void RTreeViewNode::setAllNodesUnticked()
{
  isTicked = false;
  for(int i=0; i<childNodes.size(); i++)
    childNodes[i]->setAllNodesUnticked();
}

/*
void RTreeViewNode::setAllNodesWithMatchingIdentifierTicked(int identifierToMatch)
{
  for(int i=0; i<childNodes.size(); i++)
  {
    if( childNodes[i]->getNodeIdentifier() == identifierToMatch )
      childNodes[i]->setTicked(true);
  }
  for(int i=0; i<childNodes.size(); i++)
    childNodes[i]->setAllNodesWithMatchingIdentifierTicked(identifierToMatch);
}
*/

void RTreeViewNode::addChildNode(RTreeViewNode *nodeToAdd)
{
  childNodes.add(nodeToAdd);
  nodeToAdd->setParentNode(this);
}

void RTreeViewNode::deleteChildNodesRecursively()
{
  for(int i=0; i<childNodes.size(); i++)
  {
    childNodes[i]->deleteChildNodesRecursively();
    delete childNodes[i];
  }
  childNodes.clear();
}

bool RTreeViewNode::hasAnyChildNodeChildNodes() const
{
  for(int i=0; i<childNodes.size(); i++)
  {
    if( childNodes[i]->hasChildNodes() )
      return true;
  }
  return false;
}

int RTreeViewNode::getLevel() const
{
  if( parentNode == NULL )
    return 0;
  else return parentNode->getLevel() + 1;
}

int RTreeViewNode::findIndexForIdentifier(int identifierToFind) const
{
  for(int i=0; i<childNodes.size(); i++)
  {
    if( identifierToFind == childNodes[i]->identifier )
      return i;
  }
  return -1;
}

RTreeViewNode* RTreeViewNode::findNodeByData(void *dataPointerToFind)
{
  if( userData == dataPointerToFind )
    return this;
  else if( isLeafNode() )
    return NULL;
  else
  {
    RTreeViewNode *result;
    for(int i=0; i<childNodes.size(); i++)   
    {
      result = childNodes[i]->findNodeByData(dataPointerToFind);
      if( result != NULL )
        return result;
    }
  }
  return NULL;
}

RTreeViewNode* RTreeViewNode::findNodeByIndentifier(int identifierToFind)
{
  if( identifier == identifierToFind )
    return this;
  else if( isLeafNode() )
    return NULL;
  else
  {
    RTreeViewNode *result;
    for(int i=0; i<childNodes.size(); i++)   
    {
      result = childNodes[i]->findNodeByIndentifier(identifierToFind);
      if( result != NULL )
        return result;
    }
  }
  return NULL;
}

RTreeViewNode* RTreeViewNode::getCopy()
{
  RTreeViewNode *copiedNode = new RTreeViewNode(nodeText, identifier, description, isEnabled, 
    isTicked, isOpen, parentNode);
  for(int i=0; i<childNodes.size(); i++)   
  {
    RTreeViewNode *copiedChild = childNodes[i]->getCopy();
    copiedNode->addChildNode(copiedChild);
  }
  return copiedNode;
}

//=================================================================================================
// class RTreeView:
   
RTreeView::RTreeView()
  : RWidget(nullptr) // preliminary
{
  rootNode                = NULL;
  drawRootNode            = true;
  openOrCloseNodesOnClick = true;
  xOffset                 = 0;
  yOffset                 = 0;
  setOpaque(true);

  leftRightScrollBar = new RScrollBar(false, getSkin());
  leftRightScrollBar->setSingleStepSize(getNodeHeight());
  leftRightScrollBar->addListener(this);

  upDownScrollBar = new RScrollBar(true, getSkin());
  upDownScrollBar->setSingleStepSize(getNodeHeight());
  upDownScrollBar->addListener(this);
}

RTreeView::~RTreeView()
{
  deleteAllChildren();
  dismissIfModal();
}

void RTreeView::setRootNode(RTreeViewNode *newRootNode)
{
  rootNode = newRootNode;
  repaint();
}

void RTreeView::setDrawRootNode(bool shouldBeDrawn)
{
  drawRootNode = shouldBeDrawn;
  repaint();
}

void RTreeView::registerTreeViewObserver(RTreeViewObserver* const observerToRegister)
{
  treeViewObservers.add(observerToRegister);
}

void RTreeView::deRegisterTreeViewObserver(RTreeViewObserver* const observerToDeRegister)
{
  treeViewObservers.removeValue(observerToDeRegister);
}

int RTreeView::getRequiredWidth(bool ignoreOpenness) const
{
  /*
  if( drawRootNode == true )
    return getNodeWidth(rootNode) + 2 * (textMargin + outlineThickness);
  else
    return getNodeWidth(rootNode) + 2 * (textMargin + outlineThickness) - textMargin - plusMinusSize;
  */

  if( rootNode == NULL )
    return 0;

  int result = getNodeWidth(rootNode, ignoreOpenness) + 2 * (textMargin + getOutlineThickness());
  if( drawRootNode == false )
  {
    result -= (textMargin + plusMinusSize);
    if( !rootNode->hasAnyChildNodeChildNodes() )
      result -= (textMargin + plusMinusSize + 2);  
      // the +2 is still somewhat arbitrary (it comes from the fact that our text-margin here is 2 pixels less than inside a combobox) 
      // \todo: clean this up
  }
  return result;
}

int RTreeView::getRequiredHeight(bool ignoreOpenness) const
{
  if( rootNode == NULL )
    return 0;

  // \todo: use a temporary variable here for 
  // getNodeHeight(rootNode, ignoreOpenness) + 2 * (textMargin + skin->outlineThickness):
  if( drawRootNode == true )
    return getNodeHeight(rootNode, ignoreOpenness) + 2 * (textMargin + getOutlineThickness());
  else
    return getNodeHeight(rootNode, ignoreOpenness) + 2 * (textMargin + getOutlineThickness()) - getNodeHeight();
}

int RTreeView::getRootNodeX() const
{
  if( rootNode == NULL )
    return 0;

  int result = xOffset + getOutlineThickness() + textMargin + plusMinusSize;

  if( drawRootNode == false && !rootNode->hasAnyChildNodeChildNodes() )
    result -= (plusMinusSize + textMargin + 2);
    // the +2 is still somewhat arbitrary (it comes from the fact that our text-margin here is 2 pixels less than inside a combobox) 
    // \todo: clean this up

  return result;
}

int RTreeView::getRootNodeY() const
{
  return yOffset + getOutlineThickness() + textMargin;
}

int RTreeView::getNodeWidth(const RTreeViewNode *node, bool ignoreOpenness) const
{
  //int textWidth     = boldFont10px.getTextPixelWidth(node->nodeText, boldFont10px.getDefaultKerning());
  int textWidth     = getFont().getStringWidth(node->nodeText);
  int childWidth    = 0;  
  int maxChildWidth = 0;
  if( node->hasChildNodes() && (node->isOpen || ignoreOpenness) )
  {
    for(int i=0; i<node->childNodes.size(); i++)
    {
      childWidth = getNodeWidth(node->childNodes[i], ignoreOpenness);
      if( childWidth > maxChildWidth )
        maxChildWidth = childWidth;
    }
  }
  return jmax(textWidth+plusMinusSize+textMargin, maxChildWidth+plusMinusSize+textMargin);
}

int RTreeView::getNodeHeight() const
{
  //return boldFont10px.getFontHeight() + lineSpacing;
  return (int) ceil(getFont().getHeight());
}

int RTreeView::getNodeHeight(const RTreeViewNode *node, bool ignoreOpenness) const
{
  int result = getNodeHeight();
  if( node->isOpen || ignoreOpenness )
  {
    for(int i=0; i<node->childNodes.size(); i++)
      result += getNodeHeight(node->childNodes[i], ignoreOpenness);
  }
  return result;
}
   
bool RTreeView::isPointClickable(int x, int y) const
{
  int t = getOutlineThickness();
  if( x < t || y < t || x > getWidth()-t || y > getHeight()-t )
    return false;

  if( upDownScrollBar->isVisible() && x > getWidth() - scrollBarThickness )
    return false;
  else if( leftRightScrollBar->isVisible() && y > getHeight() - scrollBarThickness )
    return false;
  else
    return true;
}

bool RTreeView::hasAnyNodeChildNodes() const
{
  if( rootNode == NULL )
    return false;

  for(int i=0; i < rootNode->getNumChildNodes(); i++)
  {
    if( rootNode->childNodes[i]->hasChildNodes() )
      return true;
  }
  return false;
}

bool RTreeView::isAnyNodeTicked() const
{
  if( rootNode == NULL )
    return false;

  for(int i=0; i < rootNode->getNumChildNodes(); i++)
  {
    if( rootNode->childNodes[i]->isTicked )
      return true;
  }
  return false;
}

void RTreeView::inputAttemptWhenModal()
{
  dismissIfModal();
}

void RTreeView::scrollBarMoved(RScrollBar* scrollBarThatHasMoved, double newRangeStart)
{
  if( scrollBarThatHasMoved == upDownScrollBar )
    yOffset = -roundDoubleToInt(newRangeStart);
  else if( scrollBarThatHasMoved == leftRightScrollBar )
    xOffset = -roundDoubleToInt(newRangeStart);
  repaint();
}

void RTreeView::mouseEnter(const MouseEvent &e)
{
  RWidget::mouseEnter(e);
  repaint();
}

void RTreeView::mouseExit(const MouseEvent &e)
{
  RWidget::mouseExit(e);
  repaint();
}

void RTreeView::mouseDown(const MouseEvent &e)
{
  if( !isPointClickable(e.x, e.y) )
    return;

  RTreeViewNode *nodeUnderMouse = getNodeAtY(e.y);
  if( nodeUnderMouse != NULL )
    nodeClicked(nodeUnderMouse, e, getNodeClickPosition(nodeUnderMouse, e.x));

  dismissIfModal(); // move into ModalComponent
}

void RTreeView::mouseMove(const MouseEvent &e)
{ 
  repaint();
}

void RTreeView::mouseWheelMove(const MouseEvent &e, float wheelIncrementX, float wheelIncrementY)
{
  if( upDownScrollBar->isVisible() )
    upDownScrollBar->moveScrollbarInSteps((int)-sign(wheelIncrementY));
}

void RTreeView::paint(Graphics &g)
{
  g.fillAll(getBackgroundColor());
  g.setColour(getOutlineColor());
  g.drawRect(0, 0, getWidth(), getHeight(), getOutlineThickness());

  if( rootNode == NULL )
    return;

  int x = getRootNodeX();
  int y = getRootNodeY();
  if( drawRootNode == true )
    y = drawNode(g, x - plusMinusSize, y, rootNode); 
  else
  {
    for(int i=0; i<rootNode->getNumChildNodes(); i++)
      y = drawNode(g, x - plusMinusSize, y, rootNode->childNodes[i]); 
    // \todo - here, we should check, if the mouse is over the current node, and if so, highlight 
    // it
  }
  
  // hide area that leaks through the scrollbar bottom-right corner:
  if( upDownScrollBar->isVisible() && leftRightScrollBar->isVisible() )
  {
    Rectangle<int> r(leftRightScrollBar->getRight()-getOutlineThickness(), 
      upDownScrollBar->getBottom()-getOutlineThickness(), scrollBarThickness, scrollBarThickness);
    g.setColour(getBackgroundColor());
    g.fillRect(r);
    g.setColour(getOutlineColor());
    g.drawRect(r, getOutlineThickness());
  }
}

void RTreeView::resized()
{
  updateScrollBarBoundsAndVisibility();
}

void RTreeView::showModallyAt(int screenX, int screenY, int width, int height)
{
  if( width < 1 )
    width = getRequiredWidth(false);
  if( height < 1 )
    height = getRequiredHeight(false);

  setBounds(screenX, screenY, width, height);
  Desktop::getInstance().addGlobalMouseListener(this);
  addToDesktop(ComponentPeer::windowHasDropShadow | ComponentPeer::windowIsTemporary );
  runModalLoop();
}

void RTreeView::showModallyAtMousePosition(int width, int height)
{
  Point<int> mousePosition = Desktop::getMousePosition();
  showModallyAt(mousePosition.getX(), mousePosition.getY(), width, height);
}

void RTreeView::dismissIfModal()
{
  if( isCurrentlyModal() )
  {
    Desktop::getInstance().removeGlobalMouseListener(this);
    removeFromDesktop();
    exitModalState(0);
  }
}

void RTreeView::updateScrollBarBoundsAndVisibility()
{
  // the logic to determine the available width/height and necessity for scrollbars are interrelated in a somewhat messy way - maybe 
  // someday we should factor that stuff out into a class Scrollable or a function getAvailableWidthAndHeight(int&, int&) or something:
  int availableHeight = getHeight();
  int requiredHeight  = getRequiredHeight(false);
  int availableWidth  = getWidth();
  int requiredWidth   = getRequiredWidth(false);

  bool upDownBarNeeded     = requiredHeight > availableHeight;
  bool leftRightBarNeeded  = requiredWidth  > availableWidth;
  bool leftRightBarNeeded2 = false;
  bool upDownBarNeeded2    = false;

  if( upDownBarNeeded )
  {
    availableWidth      -= scrollBarThickness;
    leftRightBarNeeded2  = requiredWidth  > availableWidth;
  }
  if( leftRightBarNeeded )
  {
    availableHeight  -= scrollBarThickness;
    upDownBarNeeded2  = requiredHeight > availableHeight;
  }

  if( (upDownBarNeeded == false) &&  (upDownBarNeeded2 == true) )
  {
    upDownBarNeeded  = true;
    availableWidth  -= scrollBarThickness;
  }
  if( (leftRightBarNeeded == false) && (leftRightBarNeeded2 == true) )
  {
    leftRightBarNeeded  = true;
    availableHeight    -= scrollBarThickness;
  }

  // OK, done with the mess - now set up the scrollbars:
  if( upDownBarNeeded )
  {
    upDownScrollBar->setVisible(true);
    upDownScrollBar->setBounds(getWidth()-scrollBarThickness, 0, scrollBarThickness, availableHeight);
    upDownScrollBar->setRangeLimits(0.0, requiredHeight);
    upDownScrollBar->setCurrentRange(-yOffset, availableHeight);
  }
  else
  {
    yOffset = 0;
    upDownScrollBar->setVisible(false);
  }

  if( leftRightBarNeeded )
  {
    leftRightScrollBar->setVisible(true);
    leftRightScrollBar->setBounds(0, getHeight()-scrollBarThickness, availableWidth, scrollBarThickness);
    leftRightScrollBar->setRangeLimits(0.0, requiredWidth);
    leftRightScrollBar->setCurrentRange(-xOffset, availableWidth);
  }
  else
  {
    xOffset = 0;
    leftRightScrollBar->setVisible(false);
  }

  if( upDownBarNeeded && leftRightBarNeeded )
  {
    upDownScrollBar->setBounds(getWidth()-scrollBarThickness, 0, scrollBarThickness, 
      availableHeight+getOutlineThickness());
    leftRightScrollBar->setBounds(0, getHeight()-scrollBarThickness, 
      availableWidth+getOutlineThickness(), scrollBarThickness);
  }
}

int RTreeView::drawNode(Graphics &g, int x, int y, const RTreeViewNode *nodeToDraw)
{
  // highlight background for ticked nodes:
  if( nodeToDraw->isTicked )
  {
    //g.setColour(getHandleColour());
    g.setColour(getMiddlegroundColor()); // is this right?
    g.fillRect(getOutlineThickness(), y, getWidth()-2*getOutlineThickness(), 
      getNodeHeight()-lineSpacing);
  }

  // semi-highlight background for nodes where the mouse is over:
  Point<int> mousePosition = getMouseXYRelative();
  if( contains(mousePosition) 
    && mousePosition.getY() >= y-lineSpacing/2 
    && mousePosition.getY() <  y+getNodeHeight()-lineSpacing/2
    && nodeToDraw->isEnabled )
  {
    //g.setColour(getHandleColour().withMultipliedAlpha(0.625f));
    g.setColour(getMiddlegroundColor().withMultipliedAlpha(0.625f)); // check this
    g.fillRect(getOutlineThickness(), y, getWidth()-2*getOutlineThickness(), getNodeHeight()-lineSpacing);
  }

  // draw the plus/minus button, if appropriate:
  if( nodeToDraw->hasChildNodes() )
  {
    float yOffset = 0.5f * (getNodeHeight() - plusMinusSize - lineSpacing);
    float yTmp    = y + yOffset;

    //g.setColour(getTextColour());
    g.setColour(getForegroundColor());
    g.drawRect((float)x, (float)yTmp, (float)plusMinusSize, (float)plusMinusSize, 1.f);

    // \todo use functions drawPlus/drawMinus:
    //if( nodeToDraw->isOpen )
    //  drawBitmapFontText(g, x+2, (int) yTmp, T("-"), &boldFont10px, getTextColour());
    //else
    //  drawBitmapFontText(g, x+2, (int) yTmp, T("+"), &boldFont10px, getTextColour());

    x += plusMinusSize + textMargin;
  }
  else
    x += plusMinusSize + textMargin;

  // draw the node text:
  //Colour textColour = getTextColour();
  Colour textColour = getForegroundColor();
  if( !nodeToDraw->isEnabled )
    textColour = textColour.withMultipliedAlpha(0.625f);

  //drawBitmapFontText(g, x, y, nodeToDraw->nodeText, &boldFont10px, textColour, boldFont10px.getDefaultKerning(), Justification::topLeft);  
  g.drawText(nodeToDraw->nodeText, x, y, getWidth(), getHeight(), Justification::topLeft, false);

  // draw child nodes recursively:
  if( nodeToDraw->hasChildNodes() && nodeToDraw->isOpen )
  {  
    y += getNodeHeight();
    for(int i=0; i<nodeToDraw->getNumChildNodes(); i++)
      y = drawNode(g, x, y, nodeToDraw->childNodes[i]);
    return y;
  }
  else
  {
    y += getNodeHeight();
    return y;
  }
}

RTreeViewNode* RTreeView::getNodeAtY(int y)
{
  if( rootNode == NULL )
    return NULL;

  int yStart = getRootNodeY();

  if( drawRootNode == false )
    yStart -= getNodeHeight();

  return getNodeAtY(y, yStart, rootNode);
}

RTreeViewNode* RTreeView::getNodeAtY(int y, int &yStart, RTreeViewNode* nodeToStartWith)
{
  if( y >= yStart-lineSpacing/2 && y < yStart+getNodeHeight()-lineSpacing/2 )
    return nodeToStartWith;
  else if( nodeToStartWith->hasChildNodes() && nodeToStartWith->isOpen )
  {
    for(int i=0; i<nodeToStartWith->childNodes.size(); i++)
    {
      yStart += getNodeHeight(); // yStart is a reference, so it gets incremented in the recursion, too
      RTreeViewNode* currentNode = getNodeAtY(y, yStart, nodeToStartWith->childNodes[i]);
      if( currentNode != NULL )
        return currentNode;
    }
    return NULL; 
  }
  else
    return NULL;
}

int RTreeView::getNodeClickPosition(RTreeViewNode *node, int pixelPositionX)
{
  int level      = node->getLevel();
  int xPlusMinus = getOutlineThickness() + textMargin + level * (plusMinusSize + textMargin);
  if( !drawRootNode )
   xPlusMinus -= (plusMinusSize + textMargin);
  int xText      = xPlusMinus + (plusMinusSize + textMargin);
  if( pixelPositionX < xPlusMinus-textMargin )
    return LEFT_TO_PLUSMINUS;
  else if( pixelPositionX < xText )
    return ON_PLUSMINUS;
  else
    return ON_TEXT;
}

void RTreeView::nodeClicked(RTreeViewNode *nodeThatWasClicked, const MouseEvent &mouseEvent, int clickPosition)
{    
  sendNodeClickNotification(nodeThatWasClicked, mouseEvent, clickPosition);
  if( openOrCloseNodesOnClick == true )
  {
    if( nodeThatWasClicked->hasChildNodes() )
      nodeThatWasClicked->isOpen = !nodeThatWasClicked->isOpen;
    updateScrollBarBoundsAndVisibility();
    repaint();
  }
}

void RTreeView::sendNodeClickNotification(RTreeViewNode *nodeThatWasClicked, const MouseEvent &mouseEvent, int clickPosition)
{
  for(int i=0; i<treeViewObservers.size(); i++)
    treeViewObservers[i]->treeNodeClicked(this, nodeThatWasClicked, mouseEvent, clickPosition);
}

void RTreeView::sendNodeChangeNotification(RTreeViewNode *nodeThatHasChanged)
{
  for(int i=0; i<treeViewObservers.size(); i++)
    treeViewObservers[i]->treeNodeChanged(this, nodeThatHasChanged);
}

//=========================================================================================================================================
// class RTreeLeafNodeSelector:

RTreeLeafNodeSelector::RTreeLeafNodeSelector()
{
  selectedNode = NULL;
}

void RTreeLeafNodeSelector::deSelectNode()
{
  selectedNode = NULL;
}

void RTreeLeafNodeSelector::selectNodeByIndex(int indexToSelect, bool sendNotification)
{
  if( rootNode == NULL )
    return;

  rootNode->setAllNodesUnticked();
  if( indexToSelect >= 0  && indexToSelect < rootNode->getNumChildNodes() )
  {
    rootNode->childNodes[indexToSelect]->setTicked(true);
    selectedNode = rootNode->childNodes[indexToSelect];
  }
  else
    selectedNode = NULL;

  if( sendNotification == true )
    sendNodeChangeNotification(rootNode->childNodes[indexToSelect]);
}

void RTreeLeafNodeSelector::selectNodeByIdentifier(int nodeIdentifierToSelect, bool sendNotification)
{
  if( rootNode == NULL )
    return;

  rootNode->setAllNodesUnticked();
  RTreeViewNode *selectedNode = rootNode->findNodeByIndentifier(nodeIdentifierToSelect);
  if( selectedNode != NULL )
    selectedNode->setTicked(true);

  if( sendNotification == true )
    sendNodeChangeNotification(selectedNode);
}

void RTreeLeafNodeSelector::nodeClicked(RTreeViewNode *nodeThatWasClicked, const MouseEvent &mouseEvent, int clickPosition)
{  
  if( rootNode == NULL )
    return;

  if( !nodeThatWasClicked->hasChildNodes() )
  {
    rootNode->setAllNodesUnticked();
    nodeThatWasClicked->setTicked(true);
    selectedNode = nodeThatWasClicked;
  }
  RTreeView::nodeClicked(nodeThatWasClicked, mouseEvent, clickPosition);
}
