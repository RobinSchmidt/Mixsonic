#ifndef CoordinateSystem_h
#define CoordinateSystem_h

#include "Panel.h"

/**

This class is a component, intended to be used as base-class for all components that need some
underlying coordinate-system that possibly needs to be drawn on screen, such as function-plots, 
XY-pads, etc. It inherits from the Panel class and augments it's functionality by drawing 
functions for axes, grids, etc and allows for logarithmically scaled coordinate systems.

*/

class CoordinateSystem	: public Panel
{

public:

  enum captionPositions
  {
    NO_CAPTION = 0,
    TOP_CENTER,
    CENTER
  };

  enum axisPositions
  {
    INVISIBLE = 0,
    ZERO,
    LEFT,
    RIGHT,
    TOP,
    BOTTOM
  };

  enum axisAnnotationPositions
  {
    NO_ANNOTATION = 0,
    LEFT_TO_AXIS, 
    RIGHT_TO_AXIS,
    ABOVE_AXIS,
    BELOW_AXIS
  };

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  CoordinateSystem(const String &componentName =  String(T("CoordinateSystem")));   

  /** Destructor. */
  virtual ~CoordinateSystem();   

  //-----------------------------------------------------------------------------------------------
  // setup for range and scaling of the axes:

  /** Sets the interval of the horizontal coarse grid. */
  virtual void setHorizontalCoarseGridInterval(double newGridInterval); 

  /** Sets the visibility of the horizontal coarse grid. */
  virtual void setHorizontalCoarseGridVisible(bool shouldBeVisible); 

  /** Sets the interval and visibility of the horizontal coarse grid. */
  virtual void setHorizontalCoarseGrid(double newGridInterval, bool shouldBeVisible); 

  /** Sets the interval of the horizontal fine grid. */
  virtual void setHorizontalFineGridInterval(double newGridInterval); 

  /** Sets the visibility of the horizontal fine grid. */
  virtual void setHorizontalFineGridVisible(bool shouldBeVisible); 

  /** Sets the interval and visibility of the horizontal fine grid. */
  virtual void setHorizontalFineGrid(double newGridInterval, bool shouldBeVisible); 

  /** Sets the interval of the vertical coarse grid. */
  virtual void setVerticalCoarseGridInterval(double newGridInterval); 

  /** Sets the visibility of the vertical coarse grid. */
  virtual void setVerticalCoarseGridVisible(bool shouldBeVisible); 

  /** Sets the interval and visibility of the vertical coarse grid. */
  virtual void setVerticalCoarseGrid(double newGridInterval, bool shouldBeVisible); 

  /** Sets the interval of the vertical fine grid. */
  virtual void setVerticalFineGridInterval(double newGridInterval); 

  /** Sets the visibility of the vertical fine grid. */
  virtual void setVerticalFineGridVisible(bool shouldBeVisible); 

  /** Sets the interval and visibility of the vertical fine grid. */
  virtual void setVerticalFineGrid(double newGridInterval, bool shouldBeVisible); 

  /** Sets the interval of the radial coarse grid. */
  virtual void setRadialCoarseGridInterval(double newGridInterval); 

  /** Sets the visibility of the radial coarse grid. */
  virtual void setRadialCoarseGridVisible(bool shouldBeVisible); 

  /** Sets the interval and visibility of the radial coarse grid. */
  virtual void setRadialCoarseGrid(double newGridInterval, bool shouldBeVisible); 

  /** Sets the interval of the radial fine grid. */
  virtual void setRadialFineGridInterval(double newGridInterval); 

  /** Sets the visibility of the radial fine grid. */
  virtual void setRadialFineGridVisible(bool shouldBeVisible); 

  /** Sets the interval and visibility of the radial fine grid. */
  virtual void setRadialFineGrid(double newGridInterval, bool   shouldBeVisible); 

  /** Sets the interval of the angular coarse grid. */
  virtual void setAngularCoarseGridInterval(double newGridInterval); 

  /** Sets the visibility of the angular coarse grid. */
  virtual void setAngularCoarseGridVisible(bool shouldBeVisible); 

  /** Sets the interval and visibility of the angular coarse grid. */
  virtual void setAngularCoarseGrid(double newGridInterval, bool shouldBeVisible); 

  /** Sets the interval of the angular fine grid. */
  virtual void setAngularFineGridInterval(double newGridInterval); 

  /** Sets the visibility of the angular fine grid. */
  virtual void setAngularFineGridVisible(bool shouldBeVisible); 

  /** Sets the interval and visibility of the angular fine grid. */
  virtual void setAngularFineGrid(double newGridInterval, bool shouldBeVisible); 

  /** Decides if either the x-axis or the y-axis or both should be 
  logarithmically scaled and sets up the base for the logarithms. */
  virtual void useLogarithmicScale(bool shouldBeLogScaledX, bool shouldBeLogScaledY,
    double newLogBaseX = 2.0, double newLogBaseY = 2.0);

  /** Decides, if the x-axis should be logarithmically scaled and sets up the
  base for the logarithm. */
  virtual void useLogarithmicScaleX(bool shouldBeLogScaledX, double newLogBaseX = 2.0);

  /** Decides, if the y-axis should be logarithmically scaled and sets up the 
  base for the logarithm. */
  virtual void useLogarithmicScaleY(bool shouldBeLogScaledY, double newLogBaseY = 2.0);

  /** Sets the unit of the angle (as used by the angular grid) to degrees. If false, radiant 
  will be assumed. */
  virtual void setAngleUnitToDegrees(bool shouldBeInDegrees = true);

  //-----------------------------------------------------------------------------------------------
  // appearance setup:

  /* Triggers loading of a color-scheme from a file ColorScheme.xml - if the file doesn't exist,
  it will fall back to a default color-scheme. */
  //virtual void loadColorScheme();

  /** Sets up the colour-scheme for the CoordinateSystem. */
  virtual void setColourScheme(Colour newBackgroundColour, Colour newOutLineColour,
    Colour newAxesColour, Colour newCoarseGridColour, Colour newFineGridColour);

  /** Sets up a caption for the CoordinateSystem and the position where it should appear. */
  virtual void setCaption(const String &newCaption, int newPosition = TOP_CENTER);

  /** Sets the labels for the axes and their position. */
  virtual void setAxisLabels(const String &newLabelX, const String &newLabelY, 
    int newLabelPositionX = ABOVE_AXIS, int newLabelPositionY = RIGHT_TO_AXIS);

  /** Sets the label for the x-axis and its position. */
  virtual void setAxisLabelX(const String &newLabelX, int newLabelPositionX = ABOVE_AXIS);

  /** Sets the label for the y-axis and its position. */
  virtual void setAxisLabelY(const String &newLabelY, int newLabelPositionY = RIGHT_TO_AXIS);

  /** Sets the position of the x-axis. For possible values see enum positions. */
  virtual void setAxisPositionX(int newAxisPositionX);

  /** Sets the position of the y-axis. For possible values see enum positions. */
  virtual void setAxisPositionY(int newAxisPositionY);

  /** Sets up several x-axis parameters at once */
  virtual void setupAxisX(double newMin, double newMax, bool shouldBeLogScaled, double newLogBase, 
    int newAxisPosition, double newCoarseGridInterval, double newFineGridInterval);

  /** Sets up several y-axis parameters at once */
  virtual void setupAxisY(double newMin, double newMax, bool shouldBeLogScaled, double newLogBase, 
    int newAxisPosition, double newCoarseGridInterval, double newFineGridInterval);

  /** Switches x-value annotation between below or above the x-axis (or off). */
  virtual void setAxisValuesPositionX(int newValuesPositionX);

  /** Switches y-value annotation between left to or right to the y-axis (or off). */
  virtual void setAxisValuesPositionY(int newValuesPositionY);

  /**< This function is used to pass a function-pointer. This pointer has to be the address of a 
  function which has a double-parameter and a juce::String as return-value. The function will be 
  used to convert the values on the x-axis into corresponding strings for display. */
  virtual void setStringConversionFunctionX(String (*newConversionFunction) 
    (double valueToBeConverted) );

  /** @see setStringConversionFunctionX() - same for y-axis.  */
  virtual void setStringConversionFunctionY(String (*newConversionFunction) 
    (double valueToBeConverted) );

  /** Selects whether or not the field with the values should pop up. */
  virtual void setValueFieldPopup(bool shouldPopUp);

  //-----------------------------------------------------------------------------------------------
  // inquiry:

  /** Informs, if the horizontal coarse grid is visible. */
  virtual bool isHorizontalCoarseGridVisible() const;

  /** Informs, if the horizontal fine grid is visible. */
  virtual bool isHorizontalFineGridVisible() const;

  /** Informs, if the vertical coarse grid is visible. */
  virtual bool isVerticalCoarseGridVisible() const;

  /** Informs, if the vertical fine grid is visible. */
  virtual bool isVerticalFineGridVisible() const;

  /** Informs, if the radial coarse grid is visible. */
  virtual bool isRadialCoarseGridVisible() const;

  /** Informs, if the radial fine grid is visible. */
  virtual bool isRadialFineGridVisible() const;

  /** Informs, if the angular coarse grid is visible. */
  virtual bool isAngularCoarseGridVisible() const;

  /** Informs, if the angular fine grid is visible. */
  virtual bool isAngularFineGridVisible() const;

  /** Returns the interval of the radial coarse grid. */
  virtual double getRadialCoarseGridInterval() const;

  /** Returns the interval of the radial fine grid. */
  virtual double getRadialFineGridInterval() const;

  /** Returns the interval of the angular coarse grid. */
  virtual double getAngularCoarseGridInterval() const;

  /** Returns the interval of the angular fine grid. */
  virtual double getAngularFineGridInterval() const;

  /** Informs, whether the x-axis is logarithmically scaled or not. */
  virtual bool isLogScaledX() const;

  /** Informs, whether the y-axis is logarithmically scaled or not. */
  virtual bool isLogScaledY() const;

  /** Returns the drawing as SVG compliant XmlElement. The caller must take care to delete the 
  pointer to the XmlElement when it's not needed anymore. */
  virtual XmlElement* getPlotAsSVG(int width, int height) const;

  /** Renders the plot to an image object of given width and height. The caller must take care to 
  delete the pointer to the image when it's not needed anymore. */
  virtual Image* getPlotAsImage(int width, int height) const;

  //-----------------------------------------------------------------------------------------------
  // coordinate transformations:

  /** Function for converting the x- and y-coordinate values into the corresponding coordinates 
  in an image. */
  virtual void transformToImageCoordinates(double &x, double &y, const Image* theImage) const;

  /** Function for converting the x- and y-coordinate values measured in the image's coordinate
  system to the corresponding coordinates of our plot. */
  virtual void transformFromImageCoordinates(double &x, double &y, const Image* theImage) const;

  /** Function for converting the x- and y-coordinate values into the corresponding coordinates in
  the component (double precision version).*/
  virtual void transformToComponentsCoordinates(double &x, double &y) const;

  /** Function for converting the x- and y-coordinate values into the corresponding coordinates in 
  the component (single precision version).*/
  virtual void transformToComponentsCoordinates(float &x, float &y) const;

  /** Function for converting the x- and y-coordinate values measured in the components coordinate
  system to the corresponding coordinates of our plot (double precision version). */
  virtual void transformFromComponentsCoordinates(double &x, double &y) const;

  /** Function for converting the x- and y-coordinate values measured in the components coordinate
  system to the corresponding coordinates of our plot (single precision version). */
  virtual void transformFromComponentsCoordinates(float &x, float &y) const;

  //-----------------------------------------------------------------------------------------------
  // callbacks:

  /** Lets a context menu pop up when the right button is clicked to allow export of the content 
  as image or svg drawing. */
  virtual void mouseDown(const MouseEvent& e);

  /** Overrides mouseEnter for displaying the inspection Label. */
  virtual void mouseEnter(const MouseEvent& e);

  /** Overrides mouseMove for displaying the inspection Label. */
  virtual void mouseMove(const MouseEvent& e);

  /** Overrides mouseExit for displaying the inspection Label. */
  virtual void mouseExit(const MouseEvent& e);

  /** Overrides mouseDrag to let it be called by a CoordinateSystemZoomer (override it in 
  subclasses, if you want to respond to mouseDrag events). */
  virtual void mouseDrag(const MouseEvent& e);

  /** Overrides mouseUp to let it be called by a CoordinateSystemZoomer (override it in 
  subclasses, if you want to respond to mouseUp events). */
  virtual void mouseUp(const MouseEvent& e);

  /** Overrides mouseDoubleClick to let it be called by a CoordinateSystemZoomer (override it in 
  subclasses, if you want to respond to mouseDoubleClick events). */
  virtual void mouseDoubleClick(const MouseEvent& e);

  /** Overrides mouseWheelMove to let it be called by a CoordinateSystemZoomer (override it in 
  subclasses, if you want to respond to mouseWheelMove events). */
  virtual void mouseWheelMove(const MouseEvent &e, float wheelIncrementX, float wheelIncrementY);

  /** Overrides the resized()-function of the component base-class. */
  //virtual void resized();

  //-----------------------------------------------------------------------------------------------
  // state-management:

  /** Creates an XmlElement from the current state and returns it. */
  virtual XmlElement* getStateAsXml(const String& stateName) const;

  /** Restores a state based on an XmlElement which should have been created
  with the getStateAsXml()-function. */
  virtual void setStateFromXml(const XmlElement &xmlState);

  //===============================================================================================
  juce_UseDebuggingNewOperator;

protected:

  /** Updates the scale-factors which are needed when transforming from the CoordinateSystem's 
  coordinates to Component's coordinates and vice versa. Will be called by setBounds(), 
  setRange() and useLogarithmicScale(). */
  virtual void updateScaleFactors();

  /** Overrides the method inherited from Panel in order to draw the axes, grids, etc. */
  virtual void renderClientAreaImage(bool setDirtyFlagToFalse, bool triggerRepaint);

  //-----------------------------------------------------------------------------------------------
  // drawing:

  /** Draws all the stuff either on the internal image which will be displayed as the components 
  content or on an arbitrary image (if only the first optional pointer argumnet is nonzero) or on
  an arbitrary image and an SVG compliant XmlElement (if both poiters are nonzero). */
  virtual void drawCoordinateSystem(Graphics &g, Image* targetImage = NULL, 
    XmlElement* targetSVG = NULL) const;

  /** Draws a horizontal grid with a given interval in a given colour. Gets called by 
  drawCoordinateSystem(). */
  virtual void drawHorizontalGrid(Graphics &g, double interval, 
    bool exponentialSpacing, Colour gridColour, float lineThickness, 
    Image* targetImage = NULL, XmlElement* targetSVG = NULL) const;

  /** Draws a vertical grid with a given interval in a given colour. Gets called by 
  drawCoordinateSystem(). */
  virtual void drawVerticalGrid(Graphics &g, double interval, 
    bool exponentialSpacing, Colour gridColour, float lineThickness, 
    Image* targetImage = NULL, XmlElement* targetSVG = NULL) const;

  /** Draws a radial grid with a given interval in a given colour. Gets called by 
  drawCoordinateSystem(). */
  virtual void drawRadialGrid(Graphics &g, double interval, bool exponentialSpacing, 
    Colour gridColour, float lineThickness, Image* targetImage = NULL, 
    XmlElement* targetSVG = NULL) const;

  /** Draws an angular grid with a given interval in a given colour. Gets called by 
  drawCoordinateSystem(). */
  virtual void drawAngularGrid(Graphics &g, double interval, Colour gridColour, 
    float lineThickness, Image* targetImage = NULL, XmlElement* targetSVG = NULL) const;

  /** Draws the caption/headline. Gets called by drawCoordinateSystem(). */
  virtual void drawCaption(Graphics &g, Image* targetImage = NULL, 
    XmlElement* targetSVG = NULL) const;

  /** Draws the x-axis. Gets called by drawCoordinateSystem(). */
  virtual void drawAxisX(Graphics &g, Image* targetImage = NULL, 
    XmlElement* targetSVG = NULL) const;

  /** Draws the y-axis. Gets called by drawCoordinateSystem(). */
  virtual void drawAxisY(Graphics &g, Image* targetImage = NULL, 
    XmlElement* targetSVG = NULL) const;

  /** Draws the x-axis' label. Gets called by drawCoordinateSystem(). */
  virtual void drawAxisLabelX(Graphics &g, Image* targetImage = NULL, 
    XmlElement* targetSVG = NULL) const;

  /** Draws the y-axis' label. Gets called by drawCoordinateSystem(). */
  virtual void drawAxisLabelY(Graphics &g, Image* targetImage = NULL, 
    XmlElement* targetSVG = NULL) const;

  /** Draws the numeric values at the x-axis. Gets called by drawCoordinateSystem(). */
  virtual void drawAxisValuesX(Graphics &g, Image* targetImage = NULL, 
    XmlElement* targetSVG = NULL) const;

  /** Draws the numeric values at the y-axis. Gets called by drawCoordinateSystem(). */
  virtual void drawAxisValuesY(Graphics &g, Image* targetImage = NULL, 
    XmlElement* targetSVG = NULL) const;

  /** Adds a line to an SVG drawing. */
  virtual void addLineToSvgDrawing(XmlElement* theSVG, float x1, float y1, float x2, float y2, 
    float thickness, Colour colour, bool withArrowHead = false) const;

  /** Adds text-string to an SVG drawing. */
  virtual void addTextToSvgDrawing(XmlElement* theSVG, String theText, float x, float y, 
    Justification justification = Justification::centredLeft ) const;

  /** Returns either the height of this component or the height of the image (if the pointer is 
  non-NULL). */
  double getPlotHeight(Image *targetImage = NULL) const;

  /** Returns either the height of this component or the height of the image (if the pointer is 
  non-NULL). */
  double getPlotWidth(Image *targetImage = NULL) const;

  //-----------------------------------------------------------------------------------------------
  // protected data members:

  double pixelsPerIntervalX;
  double pixelsPerIntervalY;

  int    axisPositionX;
  int    axisPositionY;
  int    axisLabelPositionX;
  int    axisLabelPositionY;
  int    axisValuesPositionX;
  int    axisValuesPositionY;

  String axisLabelX;
  String axisLabelY;

  bool   horizontalCoarseGridIsVisible;
  bool   horizontalFineGridIsVisible;
  bool   verticalCoarseGridIsVisible;
  bool   verticalFineGridIsVisible;
  bool   radialCoarseGridIsVisible;
  bool   radialFineGridIsVisible;
  bool   angularCoarseGridIsVisible;
  bool   angularFineGridIsVisible;

  double radialCoarseGridInterval;
  double radialFineGridInterval;
  double angularCoarseGridInterval;
  double angularFineGridInterval;

  bool   angleIsInDegrees;

  bool   logScaledX;
  double logBaseX;
  bool   logScaledY;
  double logBaseY;
  bool   logScaledRadius;
  double logBaseRadius;

  bool   valuePopup;

  int          captionPosition;
  String       captionString;
  DrawableText caption; // a caption for the CoordinateSystem to be used to describe what is shown. */

  /** This is a text-field which shows up with the current coordinates when the user moves the 
  mouse-coursor over the coordinate system. */
  TextEditor* inspectionField;

  /** A pointer to the function which converts a x-value into a juce-string. */
  String (*stringConversionFunctionX) (double valueToConvert);

  /** A pointer to the function which converts a y-value into a juce-string. */
  String (*stringConversionFunctionY) (double valueToConvert);

  // color scheme management:
  static Colour backgroundColour, outlineColour, axesColour, coarseGridColour, fineGridColour;
  static int instanceCounter;

};

#endif  
