#ifndef RPopUpMenu_h
#define RPopUpMenu_h

#include "RTreeView.h"
#include "RPopUpComponent.h"
//#include "../rojue_ComponentDeletionWatcher.h"


//=================================================================================================
// class RPopUpMenu and associated observer class:

class RPopUpMenu;

/** Observer class for RPopUpMenus */

class RPopUpMenuObserver
{

public:

  virtual ~RPopUpMenuObserver() {}

  /** Callback that gets called when the user has selected a new item in a popup menu. */
  virtual void rPopUpMenuChanged(RPopUpMenu* menuThatHasChanged) = 0;

};


/** 

This class implements a popup-menu that can be made to appear somewhere on the screen.

DOES NOT YET WORK - for the time being, use class RPopUpMenuOld maybe we should abandon the idea 
of spawning more and more components but rather provide a tree-view inside a single component.

*/

class RPopUpMenu : public ROwnedPopUpComponent, public RTreeViewObserver 
{

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Creates an empty popup menu that is attached to some other component (presumably a button or 
  something to open the popup). */
  RPopUpMenu(Component *componentToAttachTo);

  /** Destructor. */
  virtual ~RPopUpMenu();

  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Adds an item to the menu. */
  //virtual void addItem(const RPopUpMenuItem& itemToAdd);

  /** Adds an item to the menu. */
  virtual void addItem(int itemResultId, const juce::String& itemText, bool isEnabled = true, 
    bool isTicked = false);

  /** Adds a node-item to the menu - this may have sub-items and can be used to create 
  tree-structured popup menus. */
  virtual void addTreeNodeItem(RTreeViewNode *nodeToAdd);


  // \todo: allow users to create tree-structured popup menus
  //virtual void addTreeNodeItem(RTreeNode *nodeToAdd);

  /** Clears all the items in the menu. */
  virtual void clear();

  /** Changes the text for an existing item. */
  void setItemText(int index, const juce::String& newText);

  /** Enables or disables an existing item. */
  void setItemEnabled(int index, bool shouldBeEnabled);

  /** Selects the item that has the given index and optionally sends out a notification to our 
  observers. */
  virtual void selectItemByIndex(int itemIndex, bool sendNotification);

  /** Selects the first item that has the given "itemIdentifier" and optionally sends out a 
  notification to our observers. */
  virtual void selectItemByIdentifier(int itemIdentifier, bool sendNotification);

  /** Sets the item with the given index as ticked (or not). */
  //virtual void setItemTicked(int itemIndex, bool shouldBeTicked);

  /** Registers an observer that will be called when the box's content changes. */
  virtual void registerPopUpMenuObserver(RPopUpMenuObserver* const observerToRegister);

  /** Deregisters a previously-registered observer. */
  virtual void deRegisterPopUpMenuObserver(RPopUpMenuObserver* const observerToDeRegister);

  //-----------------------------------------------------------------------------------------------
  // inquiry:

  /** Returns the height that is required to show all items at once (without needing a 
  scrollbar). */
  virtual int getRequiredWidth(bool ignoreOpenness) const 
  { return treeView->getRequiredWidth(ignoreOpenness); }

  /** Returns the height that is required to show all items at once (without needing a 
  scrollbar). */
  virtual int getRequiredHeight(bool ignoreOpenness) const 
  { return treeView->getRequiredHeight(ignoreOpenness); }

  /** Returns the number of items in the menu. */
  virtual int getNumItems() const { return rootNode->getNumChildNodes(); }

  /** Returns a (pointer to) the item with given index (if the index is out of range, a NULL 
  pointer will be returned). */
  virtual RTreeViewNode* getItemByIndex(int index) const;

  /** Returns a (pointer to) the selected item if any, otherwise a NULL pointer will be 
  returned. */
  virtual RTreeViewNode* getSelectedItem() const;

  //-----------------------------------------------------------------------------------------------
  // callbacks:

  /** Overriden from RTreeViewObserver. */
  virtual void treeNodeClicked(RTreeView *treeView, RTreeViewNode *nodeThatWasClicked, 
    const MouseEvent &mouseEvent, int clickPosition);

  /** Overriden from RTreeViewObserver. */
  virtual void treeNodeChanged(RTreeView *treeView, RTreeViewNode *nodeThatHasChanged);

  /** Overriden to set up the size of the content-component. */
  virtual void resized();

  //virtual void paint(Graphics &g);

  /** Overriden to dismiss the menu on focus-loss. */
  //virtual void focusLost(FocusChangeType cause);


protected:

  /** Sends out a notification to all our observers to inform them that the selection has changed. */
  virtual void sendPopUpSelectionNotification();

  juce::Array<RPopUpMenuObserver*> popUpMenuObservers;

  RTreeViewNode         *rootNode;
  RTreeLeafNodeSelector *treeView;  // we use this as our contentComponent (as inherited from RPopUpComponent)

  JUCE_LEAK_DETECTOR(RPopUpMenu);

};

#endif  
