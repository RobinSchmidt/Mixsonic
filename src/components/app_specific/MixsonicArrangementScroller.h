#ifndef PanelScroller_h
#define PanelScroller_h

#include "../reusable/Panel.h"
#include "../widgets/RButton.h"

#include "MixsonicArrangementPanel.h"

/**

This is a class for zooming and scrolling in a Panel object. It's more or less done via the
"Decorator" pattern, so the panel does not become a child-component of this


\todo factor out a class PanelScroller - or maybe rename this class into PanelScroller
 -> no - factoring out is better - and then override buttonClicked method, maybe also resized
    to align only the +/- buttons for the vertical scrollbar

\todo rename files, move PanelScroller into separate file in directory reusable

\todo there's a lot of code-duplication because everything is available for x and y - get rid of
this duplication ...but how?



maybe let this class broadcast callback messages which we pick up in order to update the
visibility/position of the TrackControlComponents as well

*/

class PanelScroller : public Component, public ScrollBarListener, public ButtonListener

{

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. \todo:  assign the panel in the constructor..*/
  PanelScroller(const String &componentName = String("PanelScroller"));    
  //PanelScroller(Panel* panelToAssign);    

  /** Destructor. */
  virtual ~PanelScroller();  

  //-----------------------------------------------------------------------------------------------
  // setup:

  /** This function should be used to pass the Panel-object (or an object of some subclass 
  thereof) which should be shown, zoomed and scrolled. */
  virtual void assignPanel(Panel* newPanelToAssign);

  /** This function should be called when the Panel, to which we hold a pointer here, was deleted 
  for some reason. */
  virtual void invalidatePanelPointer();

   // \todo maybe get rid of these and pass the assigned panel in the constructor - rhen we may 
   // also get rid of all the if(thePanel == NULL) checks -> less code

  // add functions to pass the zoomFactorPerClickX/Y - if they are 1.0, the zoom-widgets diappear


  //virtual void setHorizontalScrollbarVisible(bool shouldBeVisible);

  //virtual void setVerticalScrollbarVisible(bool shouldBeVisible);


  //-----------------------------------------------------------------------------------------------
  // inquiry:

  virtual int getWidgetSize() const { return widgetSize; }

  //-----------------------------------------------------------------------------------------------
  // callbacks:

  virtual void scrollBarMoved(ScrollBar *scrollBarThatHasMoved, double newRangeStart);
  virtual void buttonClicked(Button *buttonThatWasClicked);

  /** Overrides the mouseWheelMove()-method of the indirect MouseListener base-class to allow 
  zooming by mouswheel. */
  virtual void mouseWheelMove(const MouseEvent& e, float  wheelIncrementX, float  wheelIncrementY);

  // additionally, we have to override all the other mouse-callbacks in order 
  // to forward them to the Panel:
  virtual void mouseMove       (const MouseEvent& e);
  virtual void mouseEnter      (const MouseEvent& e);
  virtual void mouseExit       (const MouseEvent& e);
  virtual void mouseDown       (const MouseEvent& e);
  virtual void mouseDrag       (const MouseEvent& e);
  virtual void mouseUp         (const MouseEvent& e);
  virtual void mouseDoubleClick(const MouseEvent& e);

  //-----------------------------------------------------------------------------------------------
  // panel manipulation:

  /** Causes the Panel to be shifted left or right and updates the scrollBarX. */
  virtual void shiftX(double shiftAmount);

  /** Zooms the x-coordinate in or out (according to whether the zoomFactor is larger or smaller 
  than 1) and updates the scrollBarX. The relative center determines around which (relative) 
  reference x-coordinate will be zoomed - this is useful for taking the mouse-position into 
  account when zooming via mouse-wheel ("zoom to mouse-coursor" instead of "zoom to center").  */
  virtual void zoomX(double zoomFactor, double relativeCenter = 0.5);

  /** Same as zoomX, but for y. */
  virtual void zoomY(double zoomFactor, double relativeCenter = 0.5);

  /** Zooms x-axis out maximally.  */
  virtual void zoomToAllX();

  /** Zooms y-axis out maximally.  */
  virtual void zoomToAllY();

  /** Zooms both axes out maximally.  */
  virtual void zoomToAllXY();

  //-----------------------------------------------------------------------------------------------
  // mostly for internal use:

  /** Causes the widgets (the zoom-buttons and scrollbars) to align themselves according to the 
  position and size of the Panel object, which is being manipulated */
  virtual void alignWidgetsToPanel();

  /** Updates the positions and sizes of the scrollbar thumbs. */
  virtual void updateScrollbars();

  //===============================================================================================

protected:

  /** Transforms from an x-value given in the Panel's coordinates to normalized 
  coordinates in the range 0...1 to be used by the horizontal scrollbar. */
  virtual double transformToScrollBarCoordinateX(double x) const;

  /** Same as transformToScrollBarCoordinateX, but for y. */
  virtual double transformToScrollBarCoordinateY(double y) const;

  /** Transforms from a normlized x-value in the range 0...1 to the Panel's 
  coordinates. */
  virtual double transformFromScrollBarCoordinateX(double x) const;

  /** Same as transformFromScrollBarCoordinateX, but for y. */
  virtual double transformFromScrollBarCoordinateY(double y) const;

  /** The factor by which is zoomed in/out on one click on a plus/minus button for x- and 
  y-axis seperately. */
  double zoomFactorPerClickX, zoomFactorPerClickY;

  /** The amount by which the Panel is shifted left/right or up/down on one click on 
  an arrow-button on the scrollbar. */
  double shiftPerClickX, shiftPerClickY;

  // the colour-scheme for the scrollbars (get rid of them):
  Colour scrollBarBackgroundColour;
  Colour scrollBarOutlineColour;
  Colour scrollBarThumbColour;

  // pointer to the Panel object in which we naviagte:
  Panel *thePanel;

  // widgets:
  ScrollBar *scrollBarX, *scrollBarY;
  RButton *zoomOutButtonX, *zoomToAllButtonX, *zoomInButtonX;
  RButton *zoomOutButtonY, *zoomToAllButtonY, *zoomInButtonY;
  RButton *zoomToAllButtonXY;

  int widgetSize;

};



class ArrangementScroller : public PanelScroller
{

public:

  // overrides:
  virtual void assignPanel(Panel* newPanelToAssign);
  virtual void alignWidgetsToPanel();
  virtual void zoomY(double zoomFactor, double relativeCenter = 0.5);
  virtual void zoomToAllY();
  virtual void mouseWheelMove(const MouseEvent& e, float  wheelIncrementX, float  wheelIncrementY);


  virtual void paintOverChildren(Graphics &g);

protected:


  MixsonicArrangementPanel *theArrangementPanel;

};


#endif  