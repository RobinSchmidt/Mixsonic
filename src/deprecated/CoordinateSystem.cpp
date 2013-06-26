#include "CoordinateSystem.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction and static member initialization:

int    CoordinateSystem::instanceCounter  = 0;
Colour CoordinateSystem::backgroundColour = Colour(0xffffffff);
Colour CoordinateSystem::outlineColour    = Colour(0xff000000);
Colour CoordinateSystem::axesColour       = Colour(0xff000000);
Colour CoordinateSystem::coarseGridColour = Colour(0xff808080);
Colour CoordinateSystem::fineGridColour   = Colour(0xffd3d3d3);

CoordinateSystem::CoordinateSystem(const String &componentName) 
: Panel(componentName)
{
  pixelsPerIntervalX            =  100.0;
  pixelsPerIntervalY            =  100.0;

  caption.setText(String::empty, Font(16));
  captionPosition               =  NO_CAPTION;
  axisPositionX                 =  ZERO;
  axisPositionY                 =  ZERO;
  axisLabelPositionX            =  ABOVE_AXIS;
  axisLabelPositionY            =  RIGHT_TO_AXIS;
  axisValuesPositionX           =  BELOW_AXIS;
  axisValuesPositionY           =  LEFT_TO_AXIS;

  axisLabelX                    =  String(T("x"));
  axisLabelY                    =  String(T("y"));

  horizontalCoarseGridIsVisible =  false;
  horizontalFineGridIsVisible	  =  false;
  verticalCoarseGridIsVisible   =  false;
  verticalFineGridIsVisible	    =  false;
  radialCoarseGridIsVisible     =  false;
  radialFineGridIsVisible       =  false;
  angularCoarseGridIsVisible    =  false;
  angularFineGridIsVisible      =  false;

  radialCoarseGridInterval      =  1.0;
  radialFineGridInterval        =  0.1;
  angularCoarseGridInterval     =  15.0;  // 15 degrees
  angularFineGridInterval       =  5.0;   // 5 degrees

  angleIsInDegrees              =  true;

  logScaledX	                  =  false;
  logBaseX	                    =  2.0;
  logScaledY	                  =  false;
  logBaseY	                    =  2.0;
  logScaledRadius               =  false;
  logBaseRadius                 =  2.0;

  // initialize the function-pointers for value->string conversion
  stringConversionFunctionX = &valueToString0;
  stringConversionFunctionY = &valueToString0;

  valuePopup = true;

  // set up the colour-scheme:
  instanceCounter++;
  //if( instanceCounter == 1 )
  //  loadColorScheme();
  caption.setColour(axesColour);

  // the Label for inspecting the current frequency ane level:
  inspectionField = new TextEditor( String(T("inspectionField")) );
  inspectionField->setBounds(4, 4, 120, 40);
  inspectionField->setColour(TextEditor::backgroundColourId, Colours::white.withAlpha(0.7f) );
  inspectionField->setColour(TextEditor::outlineColourId,    Colours::black );
  inspectionField->setCaretVisible(false);
  inspectionField->setScrollbarsShown(false);
  inspectionField->setReadOnly(true);
  inspectionField->setMultiLine(true);
  addChildComponent( inspectionField );
}

CoordinateSystem::~CoordinateSystem()
{
  instanceCounter--;
  deleteAllChildren();
}

//-------------------------------------------------------------------------------------------------
// setup:

void CoordinateSystem::setHorizontalCoarseGridInterval(double newGridInterval)
{
  setHorizontalCoarseGrid(newGridInterval, horizontalCoarseGridIsVisible);
  setDirty();
}

void CoordinateSystem::setHorizontalCoarseGridVisible(bool shouldBeVisible)
{
  setHorizontalCoarseGrid(horizontalCoarseGridInterval, shouldBeVisible);
  setDirty();
}

void CoordinateSystem::setHorizontalCoarseGrid(double newGridInterval, bool shouldBeVisible)
{
  // for logarithmic scaling of an axis, we need the grid-intervals to be 
  // strictly greater than unity because the coordinate of a grid-line results
  // from the coordinate of the previous grid-line via multiplication - we 
  // would end up drawing an infinite number of grid-lines at the same 
  // coordinate with a unity-factor and denser and denser grid-lines when 
  // approaching zero with a factor lower than unity.
  if( logScaledY )
  {
    jassert(newGridInterval > 1.00001);
    if( newGridInterval <= 1.00001 )
    {
      horizontalCoarseGridInterval = 2.0;
      return;
    }
  }
  else
  {
    jassert(newGridInterval > 0.000001); 
    // grid-intervals must be > 0
    if( newGridInterval <= 0.000001 )
      return;
  }

  horizontalCoarseGridIsVisible = shouldBeVisible;
  horizontalCoarseGridInterval  = newGridInterval;

  setDirty();
}

void CoordinateSystem::setHorizontalFineGridInterval(double newGridInterval)
{
  setHorizontalFineGrid(newGridInterval, horizontalFineGridIsVisible);
  setDirty();
}

void CoordinateSystem::setHorizontalFineGridVisible(bool shouldBeVisible)
{
  setHorizontalFineGrid(horizontalFineGridInterval, shouldBeVisible);
  setDirty();
}

void CoordinateSystem::setHorizontalFineGrid(double newGridInterval, bool shouldBeVisible)
{
  if( logScaledY )
  {
    jassert(newGridInterval > 1.00001);
    // for logarithmic scaling, we need the grid-intervals to be > 1
    if( newGridInterval <= 1.00001 )
    {
      horizontalFineGridInterval = pow(2.0, 1.0/3.0);
      return;
    }
  }
  else
  {
    jassert(newGridInterval > 0.000001); 
    // grid-intervals must be > 0
    if( newGridInterval <= 0.000001 )
      return;
  }

  horizontalFineGridIsVisible = shouldBeVisible;
  horizontalFineGridInterval  = newGridInterval;

  setDirty();
}

void CoordinateSystem::setVerticalCoarseGridInterval(double newGridInterval)
{
  setVerticalCoarseGrid(newGridInterval, verticalCoarseGridIsVisible);
  setDirty();
}

void CoordinateSystem::setVerticalCoarseGridVisible(bool shouldBeVisible)
{
  setVerticalCoarseGrid(verticalCoarseGridInterval, shouldBeVisible);
  setDirty();
}

void CoordinateSystem::setVerticalCoarseGrid(double newGridInterval, bool shouldBeVisible)
{
  if( logScaledX )
  {
    jassert(newGridInterval > 1.00001);
    // for logarithmic scaling, we need the grid-intervals to be > 1
    if( newGridInterval <= 1.00001 )
    {
      verticalCoarseGridInterval = 2.0;
      return;
    }
  }
  else
  {
    jassert(newGridInterval > 0.000001); 
    // grid-intervals must be > 0
    if( newGridInterval <= 0.000001 )
      return;
  }

  verticalCoarseGridIsVisible = shouldBeVisible;
  verticalCoarseGridInterval  = newGridInterval;

  setDirty();
}

void CoordinateSystem::setVerticalFineGridInterval(double newGridInterval)
{
  setVerticalFineGrid(newGridInterval, verticalFineGridIsVisible);
  setDirty();
}

void CoordinateSystem::setVerticalFineGridVisible(bool shouldBeVisible)
{
  setVerticalFineGrid(verticalFineGridInterval, shouldBeVisible);
  setDirty();
}

void CoordinateSystem::setVerticalFineGrid(double newGridInterval, bool shouldBeVisible)
{
  if( logScaledX )
  {
    jassert(newGridInterval > 1.00001);
    // for logarithmic scaling, we need the grid-intervals to be > 1
    if( newGridInterval <= 1.00001 )
    {
      verticalFineGridInterval = pow(2.0, 1.0/3.0);
      return;
    }
  }
  else
  {
    jassert(newGridInterval > 0.000001); 
    // grid-intervals must be > 0
    if( newGridInterval <= 0.000001 )
      return;
  }

  verticalFineGridIsVisible = shouldBeVisible;
  verticalFineGridInterval  = newGridInterval;

  setDirty();
}

void CoordinateSystem::setRadialCoarseGridInterval(double newGridInterval)
{
  setRadialCoarseGrid(newGridInterval, radialCoarseGridIsVisible);
  setDirty();
}

void CoordinateSystem::setRadialCoarseGridVisible(bool shouldBeVisible)
{
  setRadialCoarseGrid(radialCoarseGridInterval, shouldBeVisible);
  setDirty();
}

void CoordinateSystem::setRadialCoarseGrid(double newGridInterval, bool shouldBeVisible)
{
  if( logScaledRadius )
  {
    jassert(newGridInterval > 1.00001);
    // for logarithmic scaling, we need the grid-intervals to be > 1
    if( newGridInterval <= 1.00001 )
    {
      radialCoarseGridInterval = 2.0;
      return;
    }
  }
  else
  {
    jassert(newGridInterval > 0.000001); 
    // grid-intervals must be > 0
    if( newGridInterval <= 0.000001 )
      return;
  }

  radialCoarseGridIsVisible = shouldBeVisible;
  radialCoarseGridInterval  = newGridInterval;

  setDirty();
}

void CoordinateSystem::setRadialFineGridInterval(double newGridInterval)
{
  setRadialFineGrid(newGridInterval, radialFineGridIsVisible);
  setDirty();
}

void CoordinateSystem::setRadialFineGridVisible(bool shouldBeVisible)
{
  setRadialFineGrid(radialFineGridInterval, shouldBeVisible);
  setDirty();
}

void CoordinateSystem::setRadialFineGrid(double newGridInterval, bool shouldBeVisible)
{
  if( logScaledRadius )
  {
    jassert(newGridInterval > 1.00001);
    // for logarithmic scaling, we need the grid-intervals to be > 1
    if( newGridInterval <= 1.00001 )
    {
      radialFineGridInterval = pow(2.0, 1.0/3.0);
      return;
    }
  }
  else
  {
    jassert(newGridInterval > 0.000001); 
    // grid-intervals must be > 0
    if( newGridInterval <= 0.000001 )
      return;
  }

  radialFineGridIsVisible     = shouldBeVisible;
  radialFineGridInterval = newGridInterval;

  setDirty();
}

void CoordinateSystem::setAngularCoarseGridInterval(double newGridInterval)
{
  setAngularCoarseGrid(newGridInterval, angularCoarseGridIsVisible);
  setDirty();
}

void CoordinateSystem::setAngularCoarseGridVisible(bool shouldBeVisible)
{
  setAngularCoarseGrid(angularCoarseGridInterval, shouldBeVisible);
  setDirty();
}

void CoordinateSystem::setAngularCoarseGrid(double newGridInterval, bool shouldBeVisible)
{
  jassert(newGridInterval > 0.000001); 
  // grid-intervals must be > 0
  if( newGridInterval <= 0.000001 )
    return;

  angularCoarseGridIsVisible = shouldBeVisible;
  angularCoarseGridInterval  = newGridInterval;

  setDirty();
}

void CoordinateSystem::setAngularFineGridInterval(double newGridInterval)
{
  setAngularFineGrid(newGridInterval, angularFineGridIsVisible);
  setDirty();
}

void CoordinateSystem::setAngularFineGridVisible(bool shouldBeVisible)
{
  setAngularFineGrid(angularFineGridInterval, shouldBeVisible);
  setDirty();
}

void CoordinateSystem::setAngularFineGrid(double newGridInterval, bool shouldBeVisible)
{
  jassert(newGridInterval > 0.000001); 
  // grid-intervals must be > 0
  if( newGridInterval <= 0.000001 )
    return;

  angularFineGridIsVisible     = shouldBeVisible;
  angularFineGridInterval = newGridInterval;

  setDirty();
}

void CoordinateSystem::useLogarithmicScale(bool   shouldBeLogScaledX, 
                                           bool   shouldBeLogScaledY, 
                                           double newLogBaseX, 
                                           double newLogBaseY)
{
  logScaledX = shouldBeLogScaledX;
  logScaledY = shouldBeLogScaledY;
  logBaseX     = newLogBaseX;
  logBaseY     = newLogBaseY;

  updateScaleFactors();
  setDirty();
}

void CoordinateSystem::useLogarithmicScaleX(bool   shouldBeLogScaledX, 
                                            double newLogBaseX)
{
  logScaledX = shouldBeLogScaledX;
  logBaseX     = newLogBaseX;
  updateScaleFactors();
  setDirty();
}

void CoordinateSystem::useLogarithmicScaleY(bool   shouldBeLogScaledY, 
                                            double newLogBaseY)
{
  logScaledY = shouldBeLogScaledY;
  logBaseY     = newLogBaseY;
  updateScaleFactors();
  setDirty();
}

void CoordinateSystem::setAngleUnitToDegrees(bool shouldBeInDegrees)
{
  angleIsInDegrees = shouldBeInDegrees;
}

//-------------------------------------------------------------------------------------------------
// appearance setup:

/*
void CoordinateSystem::loadColorScheme()
{
  String dir = FileManager::getApplicationDirectory();
  File colorFile = File( dir + String(T("/ColorScheme.xml")) );
  if( colorFile.existsAsFile() )
  {
    XmlDocument myDocument(colorFile);
    XmlElement* xmlColorScheme = myDocument.getDocumentElement();
    String colorString;
    if( xmlColorScheme != NULL )
    {
      XmlElement* xmlColors = xmlColorScheme->getChildByName(T("Plots"));       
      if( xmlColors != NULL )
      {
        colorString = xmlColors->getStringAttribute(T("Background"), T("ffffffff") );
        backgroundColour = Colour::fromString(colorString);
        colorString = xmlColors->getStringAttribute(T("Outline"), T("ff000000") );
        outlineColour = Colour::fromString(colorString);
        colorString = xmlColors->getStringAttribute(T("Axes"), T("ff000000") );
        axesColour = Colour::fromString(colorString);
        colorString = xmlColors->getStringAttribute(T("CoarseGrid"), T("ff808080") );
        coarseGridColour = Colour::fromString(colorString);
        colorString = xmlColors->getStringAttribute(T("FineGrid"), T("ffd3d3d3") );
        fineGridColour = Colour::fromString(colorString);
      }
      caption.setColour(axesColour);
      if(autoReRenderImage == true)
        updateBackgroundImage();
      delete xmlColorScheme;
    }  
  }
}
*/

void CoordinateSystem::setColourScheme(Colour newBackgroundColour, 
                                       Colour newOutlineColour,
                                       Colour newAxesColour,
                                       Colour newCoarseGridColour,
                                       Colour newFineGridColour       )
{
  backgroundColour = newBackgroundColour;
  axesColour       = newAxesColour;
  coarseGridColour = newCoarseGridColour;
  fineGridColour   = newFineGridColour;
  outlineColour    = newOutlineColour;

  // for the caption, we use the same colour as for the axes:
  caption.setColour(axesColour);

  setDirty();
}

void CoordinateSystem::setCaption(const String &newCaption, int newPosition)
{
  captionPosition = newPosition;
  captionString   = newCaption;
  caption.setText(newCaption, Font(16));
  setDirty();
}

void CoordinateSystem::setAxisLabels(const String &newLabelX, const String &newLabelY, 
                                     int newLabelPositionX,   int newLabelPositionY)
{
  axisLabelX         = newLabelX;
  axisLabelY         = newLabelY;
  axisLabelPositionX = newLabelPositionX;
  axisLabelPositionY = newLabelPositionY;
  setDirty();
}

void CoordinateSystem::setAxisLabelX(const String& newLabelX, int newLabelPositionX)
{
  axisLabelX         = newLabelX;
  axisLabelPositionX = newLabelPositionX;
  setDirty();
}

void CoordinateSystem::setAxisLabelY(const String& newLabelY, int newLabelPositionY)
{
  axisLabelY             = newLabelY;
  axisLabelPositionY = newLabelPositionY;
  setDirty();
}

void CoordinateSystem::setAxisPositionX(int newAxisPositionX)
{
  if( newAxisPositionX == INVISIBLE ||
    newAxisPositionX == ZERO      ||
    newAxisPositionX == TOP       ||
    newAxisPositionX == BOTTOM       )
  {
    axisPositionX = newAxisPositionX;
    setDirty();
  }
}

void CoordinateSystem::setAxisPositionY(int newAxisPositionY)
{
  if( newAxisPositionY == INVISIBLE ||
    newAxisPositionY == ZERO      ||
    newAxisPositionY == LEFT      ||
    newAxisPositionY == RIGHT        )
  {
    axisPositionY = newAxisPositionY;
    setDirty();
  }
}

void CoordinateSystem::setupAxisX(double newMin, double newMax, bool shouldBeLogScaled, 
  double newLogBase, int newAxisPosition, double newCoarseGridInterval, 
  double newFineGridInterval)
{
  // axis settings seem not to make sense
  jassert( newMin < newMax )
  jassert( newMin > 0.0 || shouldBeLogScaled == false )
  jassert( shouldBeLogScaled == false || 
           (newCoarseGridInterval > 1.000001 && newFineGridInterval > 1.000001) )
  if( newMin >= newMax )
    return;
  if( newMin < 0.0 && shouldBeLogScaled == true )
    return;
  if( shouldBeLogScaled == true && 
     (newCoarseGridInterval <= 1.000001 || newFineGridInterval <= 1.000001) )
    return;

  maximumRange.setRangeX(newMin, newMax);
  currentRange.setRangeX(newMin, newMax);
  logScaledX = shouldBeLogScaled;
  logBaseX     = newLogBase;
  if( newAxisPosition == INVISIBLE ||
      newAxisPosition == ZERO      ||
      newAxisPosition == TOP       ||
      newAxisPosition == BOTTOM       )
  {
    axisPositionX = newAxisPosition;
  }
  verticalCoarseGridInterval = newCoarseGridInterval;
  verticalFineGridInterval   = newFineGridInterval;

  updateScaleFactors();
  setDirty();
}

void CoordinateSystem::setupAxisY(double newMin, double newMax, bool shouldBeLogScaled, 
  double newLogBase, int newAxisPosition, double newCoarseGridInterval, 
  double newFineGridInterval)
{
  // axis settings seem not to make sense
  jassert( newMin < newMax )
  jassert( newMin > 0.0 || shouldBeLogScaled == false )
  jassert( shouldBeLogScaled == false || 
           (newCoarseGridInterval > 1.000001 && 
            newFineGridInterval > 1.000001      )  )
  if( newMin >= newMax )
    return;
  if( newMin < 0.0 && shouldBeLogScaled == true )
    return;
  if( shouldBeLogScaled == true && 
     (newCoarseGridInterval <= 1.000001 || newFineGridInterval <= 1.000001) )
    return;

  maximumRange.setRangeY(newMin, newMax);
  currentRange.setRangeY(newMin, newMax);
  logScaledY = shouldBeLogScaled;
  logBaseY     = newLogBase;
  if( newAxisPosition == INVISIBLE ||
      newAxisPosition == ZERO      ||
      newAxisPosition == LEFT      ||
      newAxisPosition == RIGHT       )
  {
    axisPositionY = newAxisPosition;
  }
  horizontalCoarseGridInterval = newCoarseGridInterval;
  horizontalFineGridInterval   = newFineGridInterval;

  updateScaleFactors();
  setDirty();
}

void CoordinateSystem::setAxisValuesPositionX(int newValuesPositionX)
{
  if( newValuesPositionX == NO_ANNOTATION ||
    newValuesPositionX == BELOW_AXIS    ||
    newValuesPositionX == ABOVE_AXIS      )
  {
    axisValuesPositionX = newValuesPositionX;
  }
  setDirty();
}

void CoordinateSystem::setAxisValuesPositionY(int newValuesPositionY)
{
  if( newValuesPositionY == NO_ANNOTATION ||
    newValuesPositionY == LEFT_TO_AXIS  ||
    newValuesPositionY == RIGHT_TO_AXIS   )
  {
    axisValuesPositionY = newValuesPositionY;
  }
  setDirty();
}

void CoordinateSystem::setStringConversionFunctionX(
  String (*newConversionFunctionX) (double valueToBeConverted) )
{
  stringConversionFunctionX = newConversionFunctionX;
  setDirty();
}

void CoordinateSystem::setStringConversionFunctionY(
  String (*newConversionFunctionY) (double valueToBeConverted) )
{
  stringConversionFunctionY = newConversionFunctionY;
  setDirty();
}

void CoordinateSystem::setValueFieldPopup(bool shouldPopUp)
{
  valuePopup = shouldPopUp;
  inspectionField->setVisible(false);
}

//-------------------------------------------------------------------------------------------------
// inquiry:

bool CoordinateSystem::isHorizontalCoarseGridVisible() const 
{ 
  return horizontalCoarseGridIsVisible; 
}

bool CoordinateSystem::isHorizontalFineGridVisible() const 
{ 
  return horizontalFineGridIsVisible; 
}

bool CoordinateSystem::isVerticalCoarseGridVisible() const 
{ 
  return verticalCoarseGridIsVisible; 
}

bool CoordinateSystem::isVerticalFineGridVisible() const 
{ 
  return verticalFineGridIsVisible; 
}

bool CoordinateSystem::isRadialCoarseGridVisible() const 
{ 
  return radialCoarseGridIsVisible; 
}

bool CoordinateSystem::isRadialFineGridVisible() const 
{ 
  return radialFineGridIsVisible; 
}

bool CoordinateSystem::isAngularCoarseGridVisible() const 
{ 
  return angularCoarseGridIsVisible; 
}

bool CoordinateSystem::isAngularFineGridVisible() const 
{ 
  return angularFineGridIsVisible; 
}

double CoordinateSystem::getRadialCoarseGridInterval() const
{
  return radialCoarseGridInterval;
}

double CoordinateSystem::getRadialFineGridInterval() const
{
  return radialFineGridInterval;
}

double CoordinateSystem::getAngularCoarseGridInterval() const
{
  return angularCoarseGridInterval;
}

double CoordinateSystem::getAngularFineGridInterval() const
{
  return angularFineGridInterval;
}

bool CoordinateSystem::isLogScaledX() const
{
  return logScaledX;
}

bool CoordinateSystem::isLogScaledY() const
{
  return logScaledY;
}

//-------------------------------------------------------------------------------------------------
// component-overrides:

void CoordinateSystem::mouseDown(const MouseEvent &e)
{

}

void CoordinateSystem::mouseEnter(const MouseEvent &e)
{
  if( valuePopup == true )
  {
    inspectionField->setVisible(true);
    inspectionField->setTopLeftPosition(e.x, e.y);
  }
}

void CoordinateSystem::mouseMove(const MouseEvent &e)
{
  if( valuePopup == true )
  {
    inspectionField->setVisible(true);
    inspectionField->setTopLeftPosition(e.x+8, e.y-16);

    // get frequency and amplitude-level at the current mouse-position:
    double x = e.x;
    double y = e.y;
    transformFromComponentsCoordinates(x, y);

    // pass the strings to the inspectionLabel:
    inspectionField->setText(String(T("x=")) + String(x) + String::charToString(T('\n')) +
      String(T("y=")) + String(y), false);
  }
}

void CoordinateSystem::mouseExit(const MouseEvent &e)
{
  inspectionField->setVisible(false);
}

void CoordinateSystem::mouseDrag(const MouseEvent &e)
{
  Component::mouseDrag(e);
}

void CoordinateSystem::mouseUp(const MouseEvent &e)
{
  Component::mouseUp(e);
}

void CoordinateSystem::mouseDoubleClick(const MouseEvent &e)
{
  Component::mouseDoubleClick(e);
}

void CoordinateSystem::mouseWheelMove(const MouseEvent &e, float wheelIncrementX, 
                                      float wheelIncrementY)
{
  Component::mouseWheelMove(e, wheelIncrementX, wheelIncrementY);
}

/*
void CoordinateSystem::resized()
{
  updateScaleFactors();
  backgroundImageNeedsRedraw = true;  
  repaint();
  //if(autoReRenderImage == true)
  //  updateBackgroundImage();
}
*/

//-------------------------------------------------------------------------------------------------
// state saving and recall, still incomplete:

XmlElement* CoordinateSystem::getStateAsXml(const String& stateName) const
{
  XmlElement* xmlState = new XmlElement(stateName); 
  // the XmlElement which stores all the releveant state-information

  xmlState->setAttribute(String(T("MinX")), currentRange.getMinX());
  xmlState->setAttribute(String(T("MaxX")), currentRange.getMaxX());
  xmlState->setAttribute(String(T("MinY")), currentRange.getMinY());
  xmlState->setAttribute(String(T("MaxY")), currentRange.getMaxY());

  xmlState->setAttribute(String(T("HorizontalCoarseGridIsVisible")),    
    horizontalCoarseGridIsVisible);
  xmlState->setAttribute(String(T("HorizontalCoarseGridInterval")),
    horizontalCoarseGridInterval);
  xmlState->setAttribute(String(T("HorizontalFineGridIsVisible")),      
    horizontalFineGridIsVisible);
  xmlState->setAttribute(String(T("HorizontalFineGridInterval")),  
    horizontalFineGridInterval); 
  xmlState->setAttribute(String(T("VerticalCoarseGridIsVisible")),    
    verticalCoarseGridIsVisible);
  xmlState->setAttribute(String(T("VerticalCoarseGridInterval")),
    verticalCoarseGridInterval);
  xmlState->setAttribute(String(T("VerticalFineGridIsVisible")),      
    verticalFineGridIsVisible);
  xmlState->setAttribute(String(T("VerticalFineGridInterval")),  
    verticalFineGridInterval);

  return xmlState;
}

void CoordinateSystem::setStateFromXml(const XmlElement &xmlState)
{
  //bool oldAutoReRender = clientAreaImageAutoRender;
  //clientAreaImageAutoRender = false;

  currentRange.setMinX( xmlState.getDoubleAttribute(String(T("MinX")),getCurrentRangeMinX()) );
  currentRange.setMaxX( xmlState.getDoubleAttribute(String(T("MaxX")),getCurrentRangeMaxX()) );
  currentRange.setMinY( xmlState.getDoubleAttribute(String(T("MinY")),getCurrentRangeMinY()) );
  currentRange.setMaxY( xmlState.getDoubleAttribute(String(T("MaxY")),getCurrentRangeMaxY()) );

  horizontalCoarseGridIsVisible = xmlState.getBoolAttribute(
    String(T("HorizontalCoarseGridIsVisible")), isHorizontalCoarseGridVisible());
  horizontalCoarseGridInterval = xmlState.getDoubleAttribute(
    String(T("HorizontalCoarseGridInterval")), getHorizontalCoarseGridInterval());
  horizontalFineGridIsVisible = xmlState.getBoolAttribute(
    String(T("HorizontalFineGridIsVisible")), isHorizontalFineGridVisible());
  horizontalFineGridInterval = xmlState.getDoubleAttribute(
    String(T("HorizontalFineGridInterval")), getHorizontalFineGridInterval());
  verticalCoarseGridIsVisible = xmlState.getBoolAttribute(
    String(T("VerticalCoarseGridIsVisible")), isVerticalCoarseGridVisible());
  verticalCoarseGridInterval = xmlState.getDoubleAttribute(
    String(T("VerticalCoarseGridInterval")), getVerticalCoarseGridInterval());
  verticalFineGridIsVisible = xmlState.getBoolAttribute(
    String(T("VerticalFineGridIsVisible")), isVerticalFineGridVisible());
  verticalFineGridInterval = xmlState.getDoubleAttribute(
    String(T("VerticalFineGridInterval")), getVerticalFineGridInterval());

  //clientAreaImageAutoRender = oldAutoReRender;
  //repaint();
}

//-------------------------------------------------------------------------------------------------
// others:

void CoordinateSystem::updateScaleFactors()
{
  if( !logScaledX )
    scaleX = getWidth()  / (currentRange.getMaxX()-currentRange.getMinX()); 
  // scaling factor for linear plots
  else
  {
    jassert( ((currentRange.getMaxX()/currentRange.getMinX()) > 0.0) )
      // caught a logarithm of a non-positive number, make sure that the 
      // minimum and the maximum for the x-coordinate are both strictly positive
      // for logarithmic axis-scaling

      if( (currentRange.getMaxX()/currentRange.getMinX()) > 0.0 )
        pixelsPerIntervalX = getWidth()/logB((currentRange.getMaxX()/currentRange.getMinX()), 
        logBaseX);
    // the number of pixels per interval for logarithmic plots - for
    // logBase==2 this is the number of pixels per octave:
      else
        pixelsPerIntervalX = 50; // some arbitrary fallback-value
  }
  if( !logScaledY )
    scaleY = getHeight() / (currentRange.getMaxY()-currentRange.getMinY());
  else
  {
    jassert( ((currentRange.getMaxY()/currentRange.getMinY()) > 0.0) )
      // caught a logarithm of a non-positive number, make sure that the 
      // minimum and the maximum for the y-coordinate are both strictly positive
      // for logarithmic axis-scaling

      if( (currentRange.getMaxY()/currentRange.getMinY()) > 0.0 )
        pixelsPerIntervalY = getHeight()/logB((currentRange.getMaxY()/currentRange.getMinY()), 
        logBaseY);
      else
        pixelsPerIntervalY = 50; // some arbitrary fallback-value
  }
}

void CoordinateSystem::renderClientAreaImage(bool setDirtyFlagToFalse, bool triggerRepaint)
{
  if( getWidth() < 1 || getHeight() < 1 )
  {
    clientAreaImageIsDirty = false;
    return;
  }

  clientAreaImageLock.enter();

  // invoke the baseclass method to re-allocate memory if necesarry, pass false such that the 
  // baseclass leaves the dirty-flag alone:
  Panel::renderClientAreaImage(false, false);

  // draw the coordinate system (axes, grids, caption, etc.):
  Graphics g(*clientAreaImage);
  drawCoordinateSystem(g);

  // set the dirty-flag to false if so chosen, and release the lock for the image:
  if( setDirtyFlagToFalse )
    clientAreaImageIsDirty = false;

  clientAreaImageLock.exit();

  // trigger a delayed repaint, if so chosen:
  if( triggerRepaint )
    startTimer(repaintDelayInMilliseconds);
}

//-------------------------------------------------------------------------------------------------
// coordinate transformations:

void CoordinateSystem::transformToImageCoordinates(double &x, double &y, 
                                                   const Image* theImage) const
{
  if( theImage == NULL )
  {
    transformToComponentsCoordinates(x, y);
    return;
  }

  // transform the x,y values to coordinates inside this component:
  if( logScaledX )
  {
    jassert( x > 0.0 && currentRange.getMinX() > 0.0 );
    // caught a logarithm of a non-positive number
    if( x <= 0.0 || currentRange.getMinX() <= 0.0 )
      return;

    double imagePixelsPerIntervalX = 
      theImage->getWidth()/logB((currentRange.getMaxX()/currentRange.getMinX()), logBaseX);

    x = imagePixelsPerIntervalX * logB((x/currentRange.getMinX()), logBaseX);
  }
  else
  {
    double imageScaleX = theImage->getWidth()  / (currentRange.getMaxX()-currentRange.getMinX()); 

    x -= currentRange.getMinX();	   // shift origin left/right
    x *= imageScaleX;	       // scale to fit width
  }

  if( logScaledY )
  {
    jassert( y > 0.0 && currentRange.getMinY() > 0.0 );
    // caught a logarithm of a non-positive number
    if( y <= 0.0 || currentRange.getMinY() <= 0.0 )
      return;

    double imagePixelsPerIntervalY = 
      theImage->getHeight()/logB((currentRange.getMaxY()/currentRange.getMinY()), logBaseY);

    y = imagePixelsPerIntervalY * logB((y/currentRange.getMinY()), logBaseY);
  }
  else
  {
    double imageScaleY = theImage->getHeight() / (currentRange.getMaxY()-currentRange.getMinY());

    y -= currentRange.getMinY();	// shift origin up/down
    y *= imageScaleY;	         // scale to fit height
  }

  y  = theImage->getHeight()-y;	   // invert (pixels begin at top-left)
}

void CoordinateSystem::transformFromImageCoordinates(double &x, double &y, 
                                                     const Image *theImage) const
{
  if( theImage == NULL )
  {
    transformFromComponentsCoordinates(x, y);
    return;
  }

  if( logScaledX )
  {
    double imagePixelsPerIntervalX = 
      theImage->getWidth()/logB((currentRange.getMaxX()/currentRange.getMinX()), logBaseX);
    x = currentRange.getMinX() * pow(logBaseX, (x/imagePixelsPerIntervalX));
  }
  else
  {
    double imageScaleX = theImage->getWidth()  / (currentRange.getMaxX()-currentRange.getMinX());
    x /= imageScaleX;              // scale to fit width
    x += currentRange.getMinX();    // shift origin left/right
  }
  if( logScaledY )
  {
    double imagePixelsPerIntervalY = 
      theImage->getHeight()/logB((currentRange.getMaxY()/currentRange.getMinY()), logBaseY);
    y = currentRange.getMinY() * pow(logBaseY, (y/imagePixelsPerIntervalY));
  }
  else
  {
    double imageScaleY = theImage->getHeight() / (currentRange.getMaxY()-currentRange.getMinY());
    y  = getHeight()-y; 
    y /= imageScaleY;             // scale to fit height
    y += currentRange.getMinY();    // shift origin up/down
  }
}

void CoordinateSystem::transformToComponentsCoordinates(double &x, double &y) const
{
  // transform the x,y values to coordinates inside this component:
  if( logScaledX )
  {
    jassert( x > 0.0 && currentRange.getMinX() > 0.0 );
    // caught a logarithm of a non-positive number
    if( x <= 0.0 || currentRange.getMinX() <= 0.0 )
      return;

    x = pixelsPerIntervalX * logB((x/currentRange.getMinX()), logBaseX);
  }
  else
  {
    x -= currentRange.getMinX();	// shift origin left/right
    x *= scaleX;	         // scale to fit width
  }

  if( logScaledY )
  {
    jassert( y > 0.0 && currentRange.getMinY() > 0.0 );
    // caught a logarithm of a non-positive number
    if( y <= 0.0 || currentRange.getMinY() <= 0.0 )
      return;

    y = pixelsPerIntervalY * logB((y/currentRange.getMinY()), logBaseY);
  }
  else
  {
    y -= currentRange.getMinY();	// shift origin up/down
    y *= scaleY;	         // scale to fit height
  }

  y  = getHeight()-y;	   // invert (pixels begin at top-left)
}

void CoordinateSystem::transformToComponentsCoordinates(float &x, float &y) const
{
  double xd = (double) x;
  double yd = (double) y;
  transformToComponentsCoordinates(xd, yd);
  x = (float) xd;
  y = (float) yd;
}

void CoordinateSystem::transformFromComponentsCoordinates(double &x, double &y) const
{
  if( logScaledX )
  {
    x = currentRange.getMinX() * pow(logBaseX, (x/pixelsPerIntervalX));
  }
  else
  {
    x /= scaleX;             // scale to fit width
    x += currentRange.getMinX();    // shift origin left/right
  }

  if( logScaledY )
  {
    y = currentRange.getMinY() * pow(logBaseY, (y/pixelsPerIntervalY));
  }
  else
  {
    y  = getHeight()-y; 
    y /= scaleY;             // scale to fit height
    y += currentRange.getMinY();    // shift origin up/down
  }
}

void CoordinateSystem::transformFromComponentsCoordinates(float &x, float &y) const
{
  double xd = (double) x;
  double yd = (double) y;
  transformFromComponentsCoordinates(xd, yd);
  x = (float) xd;
  y = (float) yd;
}

//-------------------------------------------------------------------------------------------------
// drawing:

void CoordinateSystem::addLineToSvgDrawing(XmlElement* theSVG, 
                                           float x1, float y1, float x2, float y2, float thickness, 
                                           Colour colour, bool withArrowHead) const
{
  if( theSVG == NULL )
    return;

  XmlElement* line = new XmlElement(String(T("line")));
  line->setAttribute(String(T("x1")), x1);
  line->setAttribute(String(T("y1")), y1);
  if( withArrowHead == true && y1 == y2 )
    line->setAttribute(String(T("x2")), x2-8);
  else
    line->setAttribute(String(T("x2")), x2);

  if( withArrowHead == true && x1 == x2 )
    line->setAttribute(String(T("y2")), y2+8);
  else
    line->setAttribute(String(T("y2")), y2);

  line->setAttribute(String(T("style")), String(T("stroke-width: ")) + String(thickness) + 
    String(T("; stroke: #")) + colour.toString().substring(2) + String(T(";")) );
  theSVG->addChildElement(line);

  if( withArrowHead == true )
  {
    XmlElement* triangle = new XmlElement(String(T("path")));

    if( y1 == y2 ) // this is a rightward arrow 
    {
      triangle->setAttribute(String(T("d")), 
        String(T("M "))   + String(x2-8) + String(T(" ")) + String(y2-4) + 
        String(T(", L ")) + String(x2-8) + String(T(" ")) + String(y2+4) + 
        String(T(", L ")) + String(x2)   + String(T(" ")) + String(y2)   + 
        String(T(", Z")) );
      triangle->setAttribute(String(T("style")), String(T("stroke: none, fill: #")) 
        + colour.toString().substring(2) + String(T(";")) );
    }
    else if( x1 == x2 ) // this is an upward arrow 
    {
      triangle->setAttribute(String(T("d")), 
        String(T("M "))   + String(x2-4) + String(T(" ")) + String(y2+8) + 
        String(T(", L ")) + String(x2+4) + String(T(" ")) + String(y2+8) + 
        String(T(", L ")) + String(x2)   + String(T(" ")) + String(y2)   + 
        String(T(", Z")) );
      triangle->setAttribute(String(T("style")), String(T("stroke: none, fill: #")) 
        + colour.toString().substring(2) + String(T(";")) );
    }

    theSVG->addChildElement(triangle);
  }
}

void CoordinateSystem::addTextToSvgDrawing(XmlElement* theSVG, String theText, float x, float y, 
                                           Justification justification) const
{
  if( theSVG == NULL )
    return;

  XmlElement* textContainer = new XmlElement(String(T("text")));
  XmlElement* text          = XmlElement::createTextElement(theText);

  String jString = String::empty;
  if( justification.getFlags() == Justification::centredLeft )
    jString = String(T("start"));
  else if( justification.getFlags() == Justification::centred )
    jString = String(T("middle"));
  else if( justification.getFlags() == Justification::centredRight )
    jString = String(T("end"));

  textContainer->setAttribute(String(T("x")), x);
  textContainer->setAttribute(String(T("y")), y);
  textContainer->setAttribute(String(T("style")), String(T("font-family: sans-serif;")) +  
    String(T(" font-size: 12px;")) + String(T(" stroke: none;")) + String(T(" fill: black;")) +
    String(T(" text-anchor: ")) + jString + String(T(";")) );
  textContainer->addChildElement(text);
  theSVG->addChildElement(textContainer);
}

Image* CoordinateSystem::getPlotAsImage(int width, int height) const
{
  jassert( width  >= 1 )
    jassert( height >= 1)
    if( width < 1 || height < 1)
      return NULL;

  Image* thePlotImage = new Image(Image::RGB, width, height, true);

  // create a graphics object which is associated with the image to perform
  // the drawing-operations
  Graphics g(*thePlotImage);

  drawCoordinateSystem(g, thePlotImage);

  return thePlotImage;
}

XmlElement* CoordinateSystem::getPlotAsSVG(int width, int height) const
{
  jassert( width  >= 1 )
    jassert( height >= 1)
    if( width < 1 || height < 1)
      return NULL;

  // create an image object as dummy:
  Image* thePlotImage = new Image(Image::RGB, width, height, true);

  // create a graphics object which is associated with the image to perform
  // the drawing-operations
  Graphics g(*thePlotImage);

  // create an XmlElement to be used for the SVG drawing:
  XmlElement* theSVG = new XmlElement(String(T("svg")));
  theSVG->setAttribute(String(T("width")), width);
  theSVG->setAttribute(String(T("height")), height);

  // draw on the SVG:
  drawCoordinateSystem(g, thePlotImage, theSVG);

  // delete the dummy image:
  delete thePlotImage; 

  return theSVG;
}

void CoordinateSystem::drawCoordinateSystem(Graphics &g, Image *targetImage, 
                                            XmlElement *targetSVG) const
{
  g.setFont(Font(14));

  // fill the background:
  g.fillAll(backgroundColour);

  // draw the grids, if desired:
  if( horizontalFineGridIsVisible )
    drawHorizontalGrid(g, horizontalFineGridInterval, logScaledY, 
    fineGridColour, 1.0f, targetImage, targetSVG);
  if( verticalFineGridIsVisible )
    drawVerticalGrid(g, verticalFineGridInterval, logScaledX, 
    fineGridColour, 1.0f, targetImage, targetSVG);
  if( radialFineGridIsVisible )
    drawRadialGrid(g, radialFineGridInterval, logScaledRadius, 
    fineGridColour, 1.0f, targetImage, targetSVG);
  if( angularFineGridIsVisible )
    drawAngularGrid(g, angularFineGridInterval, fineGridColour, 1.0f, targetImage, targetSVG);

  if( horizontalCoarseGridIsVisible )
    drawHorizontalGrid(g, horizontalCoarseGridInterval, logScaledY, 
    coarseGridColour, 1.0f, targetImage, targetSVG);
  if( verticalCoarseGridIsVisible )
    drawVerticalGrid(g, verticalCoarseGridInterval, logScaledX, 
    coarseGridColour, 1.0f, targetImage, targetSVG);
  if( radialCoarseGridIsVisible )
    drawRadialGrid(g, radialCoarseGridInterval, logScaledRadius, 
    coarseGridColour, 1.0f, targetImage, targetSVG);
  if( angularCoarseGridIsVisible )
    drawAngularGrid(g, angularCoarseGridInterval, coarseGridColour, 1.0f, targetImage, targetSVG);

  // draw the coordinate system:
  if( axisPositionX != INVISIBLE )
    drawAxisX(g, targetImage, targetSVG);
  if( axisPositionY != INVISIBLE )
    drawAxisY(g, targetImage, targetSVG);

  // draw the labels on the axes:
  if( axisPositionX != INVISIBLE && axisLabelPositionX != NO_ANNOTATION )
    drawAxisLabelX(g, targetImage, targetSVG);
  if( axisPositionY != INVISIBLE && axisLabelPositionY != NO_ANNOTATION )
    drawAxisLabelY(g, targetImage, targetSVG);

  // draw the values on the axes:
  if( axisPositionX != INVISIBLE && axisValuesPositionX != NO_ANNOTATION )
    drawAxisValuesX(g, targetImage, targetSVG);
  if( axisPositionY != INVISIBLE && axisValuesPositionY != NO_ANNOTATION )
    drawAxisValuesY(g, targetImage, targetSVG);

  // draw the caption:
  drawCaption(g, targetImage, targetSVG);

  // draw an outlining rectangle:
  g.setColour(outlineColour);
  g.drawRect(0, 0, getWidth(), getHeight(), 1);
}

void CoordinateSystem::drawCaption(Graphics &g, Image* targetImage, 
                                   XmlElement *targetSVG) const
{
  float x, y, w, h;
  caption.getBounds(x, y, w, h);
  g.setColour(axesColour);

  switch( captionPosition )
  {
  case NO_CAPTION: return;
  case TOP_CENTER:
    {
      g.setColour(backgroundColour.withAlpha(0.7f));
      g.fillRect(0.5f*getWidth()-0.5f*w-2.f, 4.f, w+4.f, h);
      g.setColour(axesColour);
      caption.drawAt(g, 0.5f*getWidth()-0.5f*w, 16);
      if( targetSVG != NULL )
      {
        float centerX;
        if( targetImage != NULL )
          centerX = 0.5f*targetImage->getWidth()-0.5f*w;
        else
          centerX = 0.5f*getWidth()-0.5f*w;
        addTextToSvgDrawing(targetSVG, captionString, centerX, 16, Justification::centred);
      }
    }
    break;
  case CENTER:
    {
      caption.drawAt(g, 0.5f*getWidth()-0.5f*w, 0.5f*getHeight()-0.5f*h+10.f );
    }
    break;
  }
}

void CoordinateSystem::drawHorizontalGrid(Graphics &g, double interval, 
                                          bool exponentialSpacing, 
                                          Colour gridColour, 
                                          float lineThickness,
                                          Image* targetImage, XmlElement *targetSVG) const
{
  if( exponentialSpacing == true )
  {
    jassert( interval >= 1.00001 );
    // grid spacing must be > 1 for exponentially spaced grid-lines
    if( interval < 1.00001 )
      return;
  }
  else
  {
    jassert( interval >= 0.000001 );
    // grid spacing must be > 0
    if( interval < 0.000001 )
      return;
  }

  g.setColour(gridColour);

  long	  i;
  double	startX, endX, startY, endY;
  double accumulator;

  String gridPathDataString;

  if( exponentialSpacing ) // draw grid with exponentially spaced lines
  {
    accumulator = interval*maximumRange.getMinY();
    while( accumulator < maximumRange.getMaxY() )
    {
      startX = currentRange.getMinX();
      endX   = currentRange.getMaxX();
      startY = accumulator;
      endY   = accumulator;

      // transform:
      if( targetImage == NULL )
      {
        transformToComponentsCoordinates(startX, startY);
        transformToComponentsCoordinates(endX, endY);
      }
      else
      {
        transformToImageCoordinates(startX, startY, targetImage);
        transformToImageCoordinates(endX, endY, targetImage);
      }

      // draw:
      g.drawLine((float)startX, (float)startY, (float)endX, (float)endY, 
        lineThickness);

      // add the line to the path which will be added to the SVG drawing:
      if( targetSVG != NULL )
      {
        gridPathDataString += String(T("M ")) + String(startX) + String(T(" ")) 
          + String(startY) + String(T(" "));
        gridPathDataString += String(T("L ")) + String(endX) + String(T(" "))   
          + String(endY) + String(T(" "));
      }

      accumulator *= interval;
    }
  }
  else // draw grid with linearly spaced lines
  {
    i = 0;
    while( i*interval < currentRange.getMaxY() )
    {
      startX = currentRange.getMinX();
      endX   = currentRange.getMaxX();
      startY = i*interval;
      endY   = i*interval;

      // transform:
      if( targetImage == NULL )
      {
        transformToComponentsCoordinates(startX, startY);
        transformToComponentsCoordinates(endX, endY);
      }
      else
      {
        transformToImageCoordinates(startX, startY, targetImage);
        transformToImageCoordinates(endX, endY, targetImage);
      }

      // draw:
      g.drawLine((float)startX, (float)startY, (float)endX, (float)endY, 
        lineThickness);

      // add the line to the path which will be added to the SVG drawing member:
      if( targetSVG != NULL )
      {
        gridPathDataString += String(T("M ")) + String(startX) + String(T(" ")) 
          + String(startY) + String(T(" "));
        gridPathDataString += String(T("L ")) + String(endX) + String(T(" "))   
          + String(endY) + String(T(" "));
      }

      i++;
    }
    i = 1;
    while( -i*interval > currentRange.getMinY() )
    {
      startX = currentRange.getMinX();
      endX   = currentRange.getMaxX();
      startY = -i*interval;
      endY   = -i*interval;

      // transform:
      if( targetImage == NULL )
      {
        transformToComponentsCoordinates(startX, startY);
        transformToComponentsCoordinates(endX, endY);
      }
      else
      {
        transformToImageCoordinates(startX, startY, targetImage);
        transformToImageCoordinates(endX, endY, targetImage);
      }

      // draw:
      g.drawLine((float)startX, (float)startY, (float)endX, (float)endY, 
        lineThickness);

      // add the line to the path which will be added to the SVG drawing member:
      if( targetSVG != NULL )
      {
        gridPathDataString += String(T("M ")) + String(startX) + String(T(" ")) 
          + String(startY) + String(T(" "));
        gridPathDataString += String(T("L ")) + String(endX) + String(T(" "))   
          + String(endY) + String(T(" "));
      }

      i++;
    } // end while
  } // end else


  if( targetSVG != NULL )
  {
    XmlElement* gridPath = new XmlElement(String(T("path")));
    gridPath->setAttribute(String(T("d")), gridPathDataString);
    gridPath->setAttribute(String(T("style")), String(T("stroke-width: ")) + String(lineThickness) + 
      String(T("; stroke: #")) + gridColour.toString().substring(2) + String(T(";")) );
    targetSVG->addChildElement(gridPath);
  }
}

void CoordinateSystem::drawVerticalGrid(Graphics &g, double interval, 
                                        bool exponentialSpacing, 
                                        Colour gridColour, 
                                        float lineThickness,
                                        Image* targetImage, XmlElement *targetSVG) const
{
  if( exponentialSpacing == true )
  {
    jassert( interval >= 1.00001 );
    // grid spacing must be > 1 for exponentially spaced grid-lines
    if( interval < 1.00001 )
      return;
  }
  else
  {
    jassert( interval >= 0.000001 );
    // grid spacing must be > 0
    if( interval < 0.000001 )
      return;
  }

  g.setColour(gridColour);

  int    	i; 
  double	 startX, endX, startY, endY;
  double  accumulator;

  String gridPathDataString;

  if( exponentialSpacing ) // draw grid with exponentially spaced lines
  {
    accumulator = interval*maximumRange.getMinX();
    while( accumulator < maximumRange.getMaxX() )
    {
      startX = accumulator;
      endX   = accumulator;
      startY = currentRange.getMinY();
      endY   = currentRange.getMaxY();

      // transform:
      if( targetImage == NULL )
      {
        transformToComponentsCoordinates(startX, startY);
        transformToComponentsCoordinates(endX, endY);
      }
      else
      {
        transformToImageCoordinates(startX, startY, targetImage);
        transformToImageCoordinates(endX, endY, targetImage);
      }

      // draw:
      g.drawLine((float)startX, (float)startY, (float)endX, (float)endY, 
        lineThickness);

      // add the line to the path which will be added to the SVG drawing:
      if( targetSVG != NULL )
      {
        gridPathDataString += String(T("M ")) + String(startX) + String(T(" ")) 
          + String(startY) + String(T(" "));
        gridPathDataString += String(T("L ")) + String(endX) + String(T(" "))   
          + String(endY) + String(T(" "));
      }

      accumulator *= interval;
    }
  }
  else // draw grid with linearly spaced lines
  {
    // draw vertical lines:
    i = 0;
    while( i*interval < currentRange.getMaxX() )
    {
      startX = i*interval;
      endX   = i*interval;
      startY = currentRange.getMinY();
      endY   = currentRange.getMaxY();

      // transform:
      if( targetImage == NULL )
      {
        transformToComponentsCoordinates(startX, startY);
        transformToComponentsCoordinates(endX, endY);
      }
      else
      {
        transformToImageCoordinates(startX, startY, targetImage);
        transformToImageCoordinates(endX, endY, targetImage);
      }

      // draw:
      g.drawLine((float)startX, (float)startY, (float)endX, (float)endY, 
        lineThickness);

      // add the line to the path which will be added to the SVG drawing:
      {
        gridPathDataString += String(T("M ")) + String(startX) + String(T(" ")) 
          + String(startY) + String(T(" "));
        gridPathDataString += String(T("L ")) + String(endX) + String(T(" "))   
          + String(endY) + String(T(" "));
      }

      i++;
    }
    i = 1;
    while( -i*interval > currentRange.getMinX() )
    {
      startX = -i*interval;
      endX   = -i*interval;
      startY = currentRange.getMinY();
      endY   = currentRange.getMaxY();

      // transform:
      if( targetImage == NULL )
      {
        transformToComponentsCoordinates(startX, startY);
        transformToComponentsCoordinates(endX, endY);
      }
      else
      {
        transformToImageCoordinates(startX, startY, targetImage);
        transformToImageCoordinates(endX, endY, targetImage);
      }

      // draw:
      g.drawLine((float)startX, (float)startY, (float)endX, (float)endY, 
        lineThickness);

      // add the line to the path which will be added to the SVG drawing:
      if( targetSVG != NULL )
      {
        gridPathDataString += String(T("M ")) + String(startX) + String(T(" ")) 
          + String(startY) + String(T(" "));
        gridPathDataString += String(T("L ")) + String(endX) + String(T(" "))   
          + String(endY) + String(T(" "));
      }

      i++;
    } // end while
  } // end else

  if( targetSVG != NULL )
  {
    XmlElement* gridPath = new XmlElement(String(T("path")));
    gridPath->setAttribute(String(T("d")), gridPathDataString);
    gridPath->setAttribute(String(T("style")), String(T("stroke-width: ")) + String(lineThickness) 
      + String(T("; stroke: #")) + gridColour.toString().substring(2) + String(T(";")) );
    targetSVG->addChildElement(gridPath);
  }
}

void CoordinateSystem::drawRadialGrid(Graphics &g, double interval, 
                                      bool exponentialSpacing, 
                                      Colour gridColour, 
                                      float lineThickness,
                                      Image* targetImage, XmlElement *targetSVG) const
{
  if( exponentialSpacing == true )
  {
    jassert( interval >= 1.00001 );
    // grid spacing must be > 1 for exponentially spaced grid-lines
    if( interval < 1.00001 )
      return;
  }
  else
  {
    jassert( interval >= 0.000001 );
    // grid spacing must be > 0
    if( interval < 0.000001 )
      return;
  }

  g.setColour(gridColour);

  // calculate the radius of the largest circle to be drawn:
  double xTmp = jmax(fabs(currentRange.getMinX()), fabs(currentRange.getMaxX()) );
  double yTmp = jmax(fabs(currentRange.getMinY()), fabs(currentRange.getMaxY()) );
  double maxRadius = sqrt(xTmp*xTmp + yTmp*yTmp);

  // calculate the center-coordinates of the circles in terms of components
  // coordinates:
  double centerX = 0.0;
  double centerY = 0.0;
  double xScaler, yScaler;
  if( targetImage == NULL )
  {
    xScaler = getWidth()  / (currentRange.getMaxX()-currentRange.getMinX());
    yScaler = getHeight() / (currentRange.getMaxY()-currentRange.getMinY());
    transformToComponentsCoordinates(centerX, centerY);
  }
  else
  {
    xScaler = targetImage->getWidth()  / (currentRange.getMaxX()-currentRange.getMinX());
    yScaler = targetImage->getHeight() / (currentRange.getMaxY()-currentRange.getMinY());
    transformToImageCoordinates(centerX, centerY, targetImage);
  }

  // draw the circles:
  int    i       = 1;
  double radius  = interval;
  double xL, xR, yT, yB;
  while( radius <= maxRadius )
  {
    // draw the circle (may deform to an ellipse depending on the scaling of the 
    // axes):
    xL = centerX - xScaler*radius;
    xR = centerX + xScaler*radius;
    yT = centerY - yScaler*radius;
    yB = centerY + yScaler*radius;
    g.drawEllipse((float)xL, (float)yT, (float)(xR-xL),(float)(yB-yT), 
      lineThickness);

    // add the circle to the svg-drawing:
    if( targetSVG != NULL )
    {
      XmlElement* ellipse = new XmlElement(String(T("ellipse")));
      ellipse->setAttribute(String(T("cx")), centerX);
      ellipse->setAttribute(String(T("cy")), centerY);
      ellipse->setAttribute(String(T("rx")), xScaler*radius);
      ellipse->setAttribute(String(T("ry")), yScaler*radius);
      ellipse->setAttribute(String(T("style")), 
        String(T("stroke-width: ")) + String(lineThickness) + 
        String(T("; stroke: #")) + gridColour.toString().substring(2) + String(T(";")) + 
        String(T("fill: none;")) );
      targetSVG->addChildElement(ellipse);
    }

    // calculate the next radius (in system-coordinates)
    i++;
    radius = interval * (double) i;
  }
}

void CoordinateSystem::drawAngularGrid(Graphics &g, double interval,
                                       Colour gridColour, 
                                       float lineThickness,
                                       Image* targetImage, XmlElement *targetSVG) const
{
  g.setColour(gridColour);

  double angleIntervalInRadiant;
  if( angleIsInDegrees )
    angleIntervalInRadiant = interval*(PI/180.0);
  else
    angleIntervalInRadiant = interval;

  String gridPathDataString;

  double angle = 0.0;
  double startX, endX, startY, endY;
  int    i     = 0;
  while( angle <= PI )
  {
    endX   = cos(angle);
    endY   = sin(angle);
    startX = -endX;
    startY = -endY;

    // prolong (or shorten) the line such that it fits into the currently visible rectangle:
    fitLineToRectangle(startX, startY, endX, endY, currentRange.getMinX(), currentRange.getMinY(), 
      currentRange.getMaxX(), currentRange.getMaxY() );

    // transform:
    if( targetImage == NULL )
    {
      transformToComponentsCoordinates(startX, startY);
      transformToComponentsCoordinates(endX, endY);
    }
    else
    {
      transformToImageCoordinates(startX, startY, targetImage);
      transformToImageCoordinates(endX, endY, targetImage);
    }

    // draw:
    g.drawLine((float)startX, (float)startY, (float)endX, (float)endY, 
      lineThickness);

    // add the line to the SVG drawing:
    if( targetSVG != NULL )
    {

      // add the line to the path which will be added to the SVG drawing member:
      gridPathDataString += String(T("M ")) + String(startX) + String(T(" ")) 
        + String(startY) + String(T(" "));
      gridPathDataString += String(T("L ")) + String(endX) + String(T(" "))   
        + String(endY) + String(T(" "));
    }

    i++;
    angle = angleIntervalInRadiant * (double) i;
  }

  if( targetSVG != NULL )
  {
    XmlElement* gridPath = new XmlElement(String(T("path")));
    gridPath->setAttribute(String(T("d")), gridPathDataString);
    gridPath->setAttribute(String(T("style")), String(T("stroke-width: ")) + String(lineThickness) 
      + String(T("; stroke: #")) + gridColour.toString().substring(2) + String(T(";")) );
    targetSVG->addChildElement(gridPath);
  }
}

void CoordinateSystem::drawAxisX(Graphics &g, Image* targetImage, XmlElement *targetSVG) const
{
  if( logScaledX == true )
  {
    jassert( verticalCoarseGridInterval >= 1.00001 );
    if( verticalCoarseGridInterval < 1.00001 )
      return;
  }
  else
  {
    jassert( verticalCoarseGridInterval >= 0.000001 );
    // grid spacing must be > 0
    if( verticalCoarseGridInterval < 0.000001 )
      return;
  }

  if( axisPositionX == INVISIBLE ) 
    return;

  g.setColour(axesColour);

  double startX, endX, startY, endY;

  startX = currentRange.getMinX();
  endX	  = currentRange.getMaxX();
  if( logScaledY )
    startY = currentRange.getMinY();
  else if( axisPositionX == ZERO )
    startY = 0.0;
  else if( axisPositionX == TOP ) 
    startY = currentRange.getMaxY();
  else if( axisPositionX == BOTTOM ) 
    startY = currentRange.getMinY();
  endY = startY;

  // transform:
  if( targetImage == NULL )
  {
    transformToComponentsCoordinates(startX, startY);
    transformToComponentsCoordinates(endX, endY);
  }
  else
  {
    transformToImageCoordinates(startX, startY, targetImage);
    transformToImageCoordinates(endX, endY, targetImage);
  }

  // include some margin for axes at the top and bottom:
  if( axisPositionX == TOP )
  {
    startY += 8;
    endY   += 8;
  }
  else if( axisPositionX == BOTTOM )
  {
    startY -= 8;
    endY   -= 8;
  }

  // draw:
  g.drawArrow((float)startX, (float)startY, (float)endX, (float)endY, 
    2.0, 8.0, 8.0);

  if( targetSVG != NULL )
    addLineToSvgDrawing(targetSVG, (float)startX, (float)startY, 
    (float) endX, (float)endY, 2.0, axesColour, true);
}

void CoordinateSystem::drawAxisY(Graphics &g, Image* targetImage, XmlElement *targetSVG) const
{
  if( logScaledY == true )
  {
    jassert( horizontalCoarseGridInterval >= 1.00001 );
    if( horizontalCoarseGridInterval < 1.00001 )
      return;
  }
  else
  {
    jassert( horizontalCoarseGridInterval >= 0.000001 );
    // grid spacing must be > 0
    if( horizontalCoarseGridInterval < 0.000001 )
      return;
  }

  if( axisPositionY == INVISIBLE ) 
    return;

  g.setColour(axesColour);

  double startX, endX, startY, endY;

  startY = currentRange.getMinY();
  endY	  = currentRange.getMaxY();

  if( logScaledX )
    startX = currentRange.getMinX();
  else if( axisPositionY == ZERO )
    startX = 0.0;
  else if( axisPositionY == LEFT ) 
    startX = currentRange.getMinX();
  else if( axisPositionY == RIGHT ) 
    startX = currentRange.getMaxX();
  endX = startX;

  // transform:
  if( targetImage == NULL )
  {
    transformToComponentsCoordinates(startX, startY);
    transformToComponentsCoordinates(endX, endY);
  }
  else
  {
    transformToImageCoordinates(startX, startY, targetImage);
    transformToImageCoordinates(endX, endY, targetImage);
  }

  // include some margin for axes at the left and right:
  if( axisPositionY == LEFT )
  {
    startX += 8;
    endX   += 8;
  }
  else if( axisPositionY == RIGHT )
  {
    startX -= 8;
    endX   -= 8;
  }

  // draw:
  g.drawArrow((float)startX, (float)startY, (float)endX, (float)endY, 
    2.0, 8.0, 8.0);

  if( targetSVG != NULL )
    addLineToSvgDrawing(targetSVG, (float)startX, (float)startY, 
    (float) endX, (float)endY, 2.0, axesColour, true);
}

void CoordinateSystem::drawAxisLabelX(Graphics &g, Image* targetImage, XmlElement *targetSVG) const
{
  if( axisLabelPositionX == NO_ANNOTATION ) 
    return;

  g.setColour(axesColour);

  double posX, posY;

  // position for the label on the x-axis:
  posX = currentRange.getMaxX();
  if( logScaledY )
    posY = currentRange.getMinY();
  else if( axisPositionX == ZERO )
    posY = 0;
  else if( axisPositionX == TOP )
    posY = currentRange.getMaxY();
  else if( axisPositionX == BOTTOM )
    posY = currentRange.getMinY();

  // transform coordinates:
  if( targetImage == NULL )
    transformToComponentsCoordinates(posX, posY);
  else
    transformToImageCoordinates(posX, posY, targetImage);

  posY += 2;

  // include some margin for axes at the top and bottom:
  if( axisPositionX == TOP )
    posY += 8;
  else if( axisPositionX == BOTTOM || axisLabelPositionX == ABOVE_AXIS )
    posY -= 28;

  g.drawText(axisLabelX, (int) posX - 100, (int) posY, 96, 16, 
    Justification::centredRight, false);

  if( targetSVG != NULL )
    addTextToSvgDrawing(targetSVG, axisLabelX, 
    (float) (posX-4), (float) (posY+16), Justification::centredRight);
}

void CoordinateSystem::drawAxisLabelY(Graphics &g, Image* targetImage, XmlElement *targetSVG) const
{
  if( axisLabelPositionY == NO_ANNOTATION )
    return;

  g.setColour(axesColour);

  double posX, posY;

  if( logScaledX )
    posX = currentRange.getMinX();
  else if( axisPositionY == ZERO )
    posX = 0.0;
  else if( axisPositionY == LEFT ) 
    posX = currentRange.getMinX();
  else if( axisPositionY == RIGHT ) 
    posX = currentRange.getMaxX();

  posY  = currentRange.getMaxY();

  // transform coordinates:
  if( targetImage == NULL )
    transformToComponentsCoordinates(posX, posY);
  else
    transformToImageCoordinates(posX, posY, targetImage);

  // include some margin for axes at the left and right:

  if( axisPositionY == LEFT || axisLabelPositionY == RIGHT_TO_AXIS )
  {
    posX += 16;
    g.drawText(axisLabelY, (int) posX, (int) posY, 512, 16, 
      Justification::centredLeft, false);

    if( targetSVG != NULL )
      addTextToSvgDrawing(targetSVG, axisLabelY, 
      (float) posX, (float) posY+12, Justification::centredLeft);
  }
  else
  {
    if( axisPositionY == ZERO )
      posX -=8;
    else
      posX -= 16;
    posX -= 512;
    g.drawText(axisLabelY, (int) posX, (int) posY, 512, 16, 
      Justification::centredRight, false);

    posX += 512;
    if( targetSVG != NULL )
      addTextToSvgDrawing(targetSVG, axisLabelY, 
      (float) posX, (float) posY+12, Justification::centredRight);
  }
}

void CoordinateSystem::drawAxisValuesX(Graphics &g, Image* targetImage, 
                                       XmlElement *targetSVG) const
{
  if( logScaledX == true )
  {
    jassert( verticalCoarseGridInterval >= 1.00001 );
    if( verticalCoarseGridInterval < 1.00001 )
      return;
  }
  else
  {
    jassert( verticalCoarseGridInterval >= 0.000001 );
    // grid spacing must be > 0
    if( verticalCoarseGridInterval < 0.000001 )
      return;
  }

  if( axisValuesPositionX == NO_ANNOTATION )
    return;

  g.setColour(axesColour);

  long	  i;
  double	posX, posY, value;
  double accumulator;
  String numberString;

  // draw values on x-axis:
  if(logScaledX)
  {
    posX	      = verticalCoarseGridInterval*maximumRange.getMinX();
    accumulator = verticalCoarseGridInterval*maximumRange.getMinX();
    while( accumulator < maximumRange.getMaxX() )
    {
      posX = value = accumulator;
      if( logScaledY )
        posY = currentRange.getMinY();
      else
      {
        if( axisPositionX == ZERO )
          posY = 0;
        else if( axisPositionX == TOP )
          posY = currentRange.getMaxY();
        else if( axisPositionX == BOTTOM )
          posY = currentRange.getMinY();
      }

      // transform:
      if( targetImage == NULL )
        transformToComponentsCoordinates(posX, posY);
      else
        transformToImageCoordinates(posX, posY, targetImage);

      // include some margin for axes at the top and bottom:
      if( axisPositionX == TOP )
        posY += 8;
      if( axisPositionX == BOTTOM )
        posY -= 8;

      // draw a small line:
      g.drawLine((float)posX, (float)(posY-4.0), (float)posX, (float)(posY+4.0), 
        1.0);
      if( targetSVG != NULL )
        addLineToSvgDrawing(targetSVG, (float)posX, (float)(posY-4.0), 
        (float)posX, (float)(posY+4.0), 1.0, axesColour, false);

      // draw number:
      numberString = stringConversionFunctionX(value);
      if( axisValuesPositionX == ABOVE_AXIS || axisPositionX == BOTTOM )
      {
        g.drawText(numberString, (int)posX-32, (int)posY-20, 64, 16, 
          Justification::centred, false);
        if( targetSVG != NULL )
          addTextToSvgDrawing(targetSVG, numberString, (float)(posX-32), (float)(posY-8), 
          Justification::centred);
      }
      else
      {
        g.drawText(numberString, (int)posX-32, (int)posY+4, 64, 16, 
          Justification::centred, false);
        if( targetSVG != NULL )
          addTextToSvgDrawing(targetSVG, numberString, (float)(posX-32), (float)(posY+16), 
          Justification::centred);
      }

      accumulator *= verticalCoarseGridInterval;
    }
  }
  else	 // x is linerarly scaled
  {
    if( axisPositionY == LEFT  || 
      axisPositionY == RIGHT || 
      axisPositionY == INVISIBLE)
      i = 0;
    else
      i = 1;
    while( i*verticalCoarseGridInterval < currentRange.getMaxX() )
    {
      posX = value = i*verticalCoarseGridInterval;
      // "value" will not be transformed
      if( logScaledY )
        posY = currentRange.getMinY();
      else
      {
        if( axisPositionX == ZERO )
          posY = 0;
        else if( axisPositionX == TOP )
          posY = currentRange.getMaxY();
        else if( axisPositionX == BOTTOM )
          posY = currentRange.getMinY();
      }

      // transform coordinates:
      if( targetImage == NULL )
        transformToComponentsCoordinates(posX, posY);
      else
        transformToImageCoordinates(posX, posY, targetImage);


      // include some margin for axes at the top and bottom:
      if( axisPositionX == TOP )
        posY += 8;
      if( axisPositionX == BOTTOM )
        posY -= 8;

      // draw a small line:
      g.drawLine((float)posX, (float)(posY-4.0), (float)posX, (float)(posY+4.0), 
        1.0);
      if( targetSVG != NULL )
        addLineToSvgDrawing(targetSVG, (float)posX, (float)(posY-4.0), 
        (float)posX, (float)(posY+4.0), 1.0, axesColour, false);

      // draw the number:
      numberString = stringConversionFunctionX(value);
      if( axisValuesPositionX == ABOVE_AXIS || axisPositionX == BOTTOM )
      {
        g.drawText(numberString, (int)posX-32, (int)posY-20, 64, 16, 
          Justification::centred, false);
        if( targetSVG != NULL )
          addTextToSvgDrawing(targetSVG, numberString, 
          (float)posX, (float)(posY-8), Justification::centred);
      }
      else
      {
        g.drawText(numberString, (int)posX-32, (int)posY+4, 64, 16, 
          Justification::centred, false);
        if( targetSVG != NULL )
          addTextToSvgDrawing(targetSVG, numberString, 
          (float)(posX), (float)(posY+16), Justification::centred);
      }

      i++;
    }
    i = 1;
    while( -i*verticalCoarseGridInterval > currentRange.getMinX() )
    {
      posX = value = -i*verticalCoarseGridInterval; 
      // "value" will not be transformed
      if( logScaledY )
        posY = currentRange.getMinY();
      else
      {
        if( axisPositionX == ZERO )
          posY = 0;
        else if( axisPositionX == TOP )
          posY = currentRange.getMaxY();
        else if( axisPositionX == BOTTOM )
          posY = currentRange.getMinY();
      }

      // transform coordinates:
      if( targetImage == NULL )
        transformToComponentsCoordinates(posX, posY);
      else
        transformToImageCoordinates(posX, posY, targetImage);

      // include some margin for axes at the top and bottom:
      if( axisPositionX == TOP )
        posY += 8;
      if( axisPositionX == BOTTOM )
        posY -= 8;

      // draw a small line:
      g.drawLine((float)posX, (float)(posY-4.0), (float)posX, (float)(posY+4.0), 
        1.0);
      if( targetSVG != NULL )
        addLineToSvgDrawing(targetSVG, (float)posX, (float)(posY-4.0), 
        (float)posX, (float)(posY+4.0), 1.0, axesColour, false);

      // draw the number:
      numberString = stringConversionFunctionX(value);
      if( axisValuesPositionX == ABOVE_AXIS || axisPositionX == BOTTOM )
      {
        g.drawText(numberString, (int)posX-32, (int)posY-20, 64, 16, 
        Justification::centred, false);
        if( targetSVG != NULL )
          addTextToSvgDrawing(targetSVG, numberString, 
          (float)posX, (float)(posY-8), Justification::centred);
      }
      else
      {
        g.drawText(numberString, (int)posX-32, (int)posY+4, 64, 16, 
        Justification::centred, false);
        if( targetSVG != NULL )
          addTextToSvgDrawing(targetSVG, numberString, 
          (float)posX, (float)(posY+16), Justification::centred);

      }

      i++;
    }
  }
}

void CoordinateSystem::drawAxisValuesY(Graphics &g, Image* targetImage, 
                                       XmlElement *targetSVG) const
{
  if( logScaledY == true )
  {
    jassert( horizontalCoarseGridInterval >= 1.00001 );
    if( horizontalCoarseGridInterval < 1.00001 )
      return;
  }
  else
  {
    jassert( horizontalCoarseGridInterval >= 0.000001 );
    // grid spacing must be > 0
    if( horizontalCoarseGridInterval < 0.000001 )
      return;
  }

  if( axisValuesPositionY == NO_ANNOTATION )
    return;

  g.setColour(axesColour);

  long	  i;
  double	posX, posY, value;
  double accumulator;
  String numberString;

  // draw values on y-axis:
  if(logScaledY)
  {
    posY	      = horizontalCoarseGridInterval*maximumRange.getMinY();
    accumulator = horizontalCoarseGridInterval*maximumRange.getMinY();
    while( accumulator < maximumRange.getMaxY() )
    {
      if( logScaledX )
        posX = currentRange.getMinX();
      else
      {
        if( axisPositionY == ZERO )
          posX = 0.0;
        else if( axisPositionY == LEFT ) 
          posX = currentRange.getMinX();
        else if( axisPositionY == RIGHT ) 
          posX = currentRange.getMaxX();
      }
      posY = value = accumulator;

      // transform:
      if( targetImage == NULL )
        transformToComponentsCoordinates(posX, posY);
      else
        transformToImageCoordinates(posX, posY, targetImage);

      // include some margin for axes at the left and right:
      if( axisPositionY == LEFT )
        posX += 8;
      else if( axisPositionY == RIGHT )
        posX -= 8;

      // draw a small line:
      g.drawLine((float)(posX-4.0), (float)posY, (float)(posX+4.0), (float)posY, 
        1.0);
      if( targetSVG != NULL )
        addLineToSvgDrawing(targetSVG, (float)(posX-4.0), (float)posY, 
        (float)(posX+4.0), (float)posY, 1.0, axesColour, false);

      // draw number:
      numberString = stringConversionFunctionY(value);
      if( axisValuesPositionY == LEFT_TO_AXIS )
      {
        g.drawText(numberString, (int)posX-25, (int)posY-10, 20, 20, 
          Justification::centredRight, false);
        if( targetSVG != NULL )
          addTextToSvgDrawing(targetSVG, numberString, 
          (float)(posX+8), (float)(posY+4), Justification::centredLeft);
      }
      else
      {
        g.drawText(numberString, (int)posX+4, (int)posY-10, 20, 20, 
          Justification::centredLeft, false);
        if( targetSVG != NULL )
          addTextToSvgDrawing(targetSVG, numberString, 
          (float)(posX-8), (float)(posY+4), Justification::centredRight);
      }

      accumulator *= horizontalCoarseGridInterval;
    }
  }
  else // y is linearly scaled
  {
    if( axisPositionX == TOP    || 
      axisPositionX == BOTTOM || 
      axisPositionX == INVISIBLE )
      i = 0;
    else
      i = 1;
    while( i*horizontalCoarseGridInterval < currentRange.getMaxY() )
    {
      if( logScaledX )
        posX = currentRange.getMinX();
      else
      {
        if( axisPositionY == ZERO )
          posX = 0.0;
        else if( axisPositionY == LEFT ) 
          posX = currentRange.getMinX();
        else if( axisPositionY == RIGHT ) 
          posX = currentRange.getMaxX();
      }
      posY = value = i*horizontalCoarseGridInterval; 
      // "value" will not be transformed

      // transform coordinates:
      if( targetImage == NULL )
        transformToComponentsCoordinates(posX, posY);
      else
        transformToImageCoordinates(posX, posY, targetImage);

      // include some margin for axes at the left and right:
      if( axisPositionY == LEFT )
        posX += 8;
      else if( axisPositionY == RIGHT )
        posX -= 8;

      // draw a small line:
      g.drawLine((float)(posX-4.0), (float)posY, (float)(posX+4.0), (float)posY, 
        1.0);
      if( targetSVG != NULL )
        addLineToSvgDrawing(targetSVG, (float)(posX-4.0), (float)posY, 
        (float)(posX+4.0), (float)posY, 1.0, axesColour, false);

      // draw number:
      numberString = stringConversionFunctionY(value);
      if( axisValuesPositionY == RIGHT_TO_AXIS || axisPositionY == LEFT )
      {
        g.drawText(numberString, (int)posX+8, (int)posY-10, 64, 20, 
          Justification::centredLeft, false);
        if( targetSVG != NULL )
          addTextToSvgDrawing(targetSVG, numberString, 
          (float)(posX+8), (float)(posY+4), Justification::centredLeft);
      }
      else
      {
        g.drawText(numberString, (int)posX-8-64, (int)posY-10, 64, 20, 
          Justification::centredRight, false);
        if( targetSVG != NULL )
          addTextToSvgDrawing(targetSVG, numberString, 
          (float)(posX-8), (float)(posY+4), Justification::centredRight);
      }

      i++;
    }
    i = 1;
    while( -i*horizontalCoarseGridInterval > currentRange.getMinY() )
    {
      if( logScaledX )
        posX = currentRange.getMinX();
      else
      {
        if( axisPositionY == ZERO )
          posX = 0.0;
        else if( axisPositionY == LEFT ) 
          posX = currentRange.getMinX();
        else if( axisPositionY == RIGHT ) 
          posX = currentRange.getMaxX();
      }
      posY = value = -i*horizontalCoarseGridInterval; // "value" will not be transformed

      // transform coordinates:
      if( targetImage == NULL )
        transformToComponentsCoordinates(posX, posY);
      else
        transformToImageCoordinates(posX, posY, targetImage);

      // include some margin for axes at the left and right:
      if( axisPositionY == LEFT )
        posX += 8;
      else if( axisPositionY == RIGHT )
        posX -= 8;

      // draw a small line:
      g.drawLine((float)(posX-4.0), (float)posY, (float)(posX+4.0), (float)posY, 
        1.0);
      if( targetSVG != NULL )
        addLineToSvgDrawing(targetSVG, (float)(posX-4.0), (float)posY, 
        (float)(posX+4.0), (float)posY, 1.0, axesColour, false);

      // draw number:
      numberString = stringConversionFunctionY(value);
      if( axisValuesPositionY == RIGHT_TO_AXIS || axisPositionY == LEFT )
      {
        g.drawText(numberString, (int)posX+8, (int)posY-10, 64, 20, 
          Justification::centredLeft, false);
        if( targetSVG != NULL )        
          addTextToSvgDrawing(targetSVG, numberString, 
          (float)(posX+8), (float)(posY+4), Justification::centredLeft);
      }
      else
      {
        g.drawText(numberString, (int)posX-8-64, (int)posY-10, 64, 20, 
          Justification::centredRight, false);
        if( targetSVG != NULL )
          addTextToSvgDrawing(targetSVG, numberString, 
          (float)(posX-8), (float)(posY+4), Justification::centredRight);
      }

      i++;
    }  // end while
  }
}

double CoordinateSystem::getPlotHeight(Image *targetImage) const
{
  if( targetImage == NULL )
    return getHeight();
  else
    return targetImage->getHeight();
}

double CoordinateSystem::getPlotWidth(Image *targetImage) const
{
  if( targetImage == NULL )
    return getWidth();
  else
    return targetImage->getWidth();
}










