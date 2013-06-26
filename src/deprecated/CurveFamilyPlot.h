#ifndef CurveFamilyPlot_h
#define CurveFamilyPlot_h

#include "CoordinateSystem.h"
#pragma warning(disable : 4250) // disable dominance warning in MSVC

/**

This class is intended to be used a baseclass for various kinds of curve- or function plots (or
more generally, curve/function family plots).

*/

class CurveFamilyPlot : virtual public CoordinateSystem
{

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  CurveFamilyPlot(const String& name = String(T("CurveFamilyPlot")));   

  /** Destructor. */
  virtual ~CurveFamilyPlot(); 

  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Accepts the new function or curve values to be drawn. The data will NOT be copied into 
  internal buffers, an object of class "CurveFamilyPlot" will only hold pointers to the data. If 
  the outlying class for some reason deletes the array in which the data is stored, it should call
  invalidatePointers() - such that the "CurveFamilyPlot"-object does not try to aceess the 
  array anymore. */
  virtual void setCurveFamilyValues(int newNumValues, int newNumCurves, 
    double**  newFamilyValuesX, double** newFamilyValuesY);

  /** Accepts the new function or curve values to be drawn. The newValuesX and newValuesY are 
  assumed to arrays of length newNumValues and this class will treat this function-call as if
  you would have called curveFamilyValues with a number of curves of one. */
  virtual void setCurveValues(int newNumValues, double* newValuesX, double* newValuesY);

  /** Accepts values for a fucntion family.  */
  virtual void setFunctionFamilyValues(int newNumValues, int newNumFunctions, 
    double* newValuesX, double** newFamilyValuesY);

  /** Sets the number of curves to be drawn. Can be used to draw not all but only the first few
  curves from the familyValuesY-array. But the number passed here should never be larger than
  the actual number of arrays residing in the memory. */
  virtual void setNumCurves(int newNumCurves) { numCurves = newNumCurves; }

  /** Initializes the color-scheme to defaults. */
  virtual void initColorScheme();

  /** You can pass an array with the colours for the different curves here. */
  virtual void setCurveColours(const OwnedArray<Colour> &newCurveColours);

  /** Invalidates the pointers to the data. */
  virtual void invalidatePointers();

  //-----------------------------------------------------------------------------------------------
  // inquiry:

  /** Returns the drawing as SVG compliant XmlElement. The caller must take care to delete the 
  pointer to the XmlElement when it's not needed anymore. */
  virtual XmlElement* getPlotAsSVG(int width, int height) const;

  /** Renders the plot to an image object of given width and height. The caller must take 
  care to delete the image when it's not needed anymore. */
  virtual Image* getPlotAsImage(int width, int height) const;

  //===============================================================================================
  juce_UseDebuggingNewOperator;

protected:

  /** Overrides the method inherited from CoordinateSystem in order to draw the curves. */
  virtual void renderClientAreaImage(bool setDirtyFlagToFalse, bool triggerRepaint);

  /** Plots the curve family either on a graphics drawing canvas or an image. */
  virtual void plotCurveFamily(Graphics &g, Image *targetImage = NULL, 
    XmlElement *targetSVG = NULL) const;

  virtual void plotFamilyValuesAsDots(Graphics &g, Image* targetImage = NULL, 
    XmlElement *targetSVG = NULL) const;

  OwnedArray<Colour> curveColours;	  // array which holds the colurs for the graphs

  int numCurves;	      // number of curves in memory
  int numValues;       	// number of values per curve
  int numCurvesToDraw;  // the number of curves which should be drawn (should be <= numCurves)

  double** familyValuesX; 
  double** familyValuesY; 
  // pointer to a two-dimensional array. first index indicates the curve, second index indicates the
  // particular y-value

  double*  valuesX1;	      // pointer to the first array of x-values
  double*  valuesY1;	      // pointer to the first array of x-values

  //double**  decimatedValuesX;
  //double*** decimatedFamilyValuesY;
  // similar to the arrays above but with recursively decimated peak-data - 
  // the first index represents the decimation level

  Colour  graphColour;
  bool	  fillAreaUnderFunction;   // good for spectra
  bool    isFunctionFamily;

};

#endif	
