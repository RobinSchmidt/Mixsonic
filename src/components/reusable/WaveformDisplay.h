#ifndef WaveformDisplay_h
#define WaveformDisplay_h

#include "../../core/audio/AudioFileBufferUser.h"
#include "ThreadedDrawingPanel.h"

/**

This class is intended to be used as a graphical display for a waveform.

\todo factor out a general function plotting class

*/

class WaveformDisplay	: public ThreadedDrawingPanel, public AudioFileBufferUser
{

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  WaveformDisplay(AudioFileBuffer *newBuffer = NULL);   

  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Sets the background color. */
  virtual void setBackgroundColor(const Colour &newColor);

  /** Sets the outline color. */
  virtual void setOutlineColor(const Colour &newColor);

  /** Sets the graph color. */
  virtual void setGraphColor(const Colour &newColor);

  /** Sets the waveform-data from an AudioSampleBuffer and informs about success. */
  virtual void assignAudioFileBuffer(AudioFileBuffer *newBuffer);

  /** Updates the sample rate for this display - this is need to display the time-axis in 
  seconds, when desired. */
  virtual void setSampleRate(double newSampleRate);
  
  /** Allows the user to restrict the number of channels to be plotted from below, counting starts 
  at 0. */
  virtual void setFirstChannelToPlot(int newFirstChannelIndex);

  /** Allows the user to restrict the number of channels to be plotted from above, counting starts 
  at 0. */
  virtual void setLastChannelToPlot(int newLastChannelIndex);

  /** Allows the user to select exactly one channel to be plotted, counting starts at 0. */
  virtual void plotOnlyOneChannel(int channelToPlotIndex);

  /* Allows to constrain the time range to be drawn. */
  virtual void setVisibleTimeRange(double newMinTimeInSeconds, double newMaxTimeInSeconds);

  /*
  //-----------------------------------------------------------------------------------------------
  // overrides:
  virtual void paint(Graphics &g);
  virtual void resized();
  virtual void setDirty(bool shouldBeSetToDirty = true);
  */

  //===============================================================================================
  juce_UseDebuggingNewOperator;

protected:
		
  /** Overrides drawComponent inherited from ThreadedDrawingComponent in order to do the actual 
  drawing operations. */
  virtual void drawComponent(Image* imageToDrawOnto);

  /** Plots the actual waveform data onto an image that is passed. */
  virtual void plotWaveform(Image *targetImage = NULL);

  /** When this component has bounds which extend beyond the parent's bounds, a part of the 
  waveform may be invisible - this function will find out the minimum and maximum time instants
  which are still visible. The margin in percent can be used to plot some percentage into the 
  invisible region (to both sides). */
  virtual void restrictToVisibleSection(double &tMin, double &tMax, 
    double marginInPercent = 10.0) const;

  // some additional parameters to control the plotting:
  int    firstChannelToPlot, lastChannelToPlot;
  double minVisibleTime, maxVisibleTime;
  int    numChannels, numSamples;
  double sampleRate; // probably to be deprecated, because redundant withe info in the bufferToUse

  Colour backgroundColor, graphColor, outlineColor;

};

#endif 
