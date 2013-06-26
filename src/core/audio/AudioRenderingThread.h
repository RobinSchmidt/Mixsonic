#ifndef AudioRenderingThread_h
#define AudioRenderingThread_h

#include "../infrastructure/Arrangement.h"

/**

This class implements a thread that renders the audio output from an Arrangement into an 
AudioSampleBuffer. The Arrangement object to be rendered must be passed via setArrangementToRender,
then you should call runThread() and if this function returns true, you should retrieve the 
rendering result via getRenderingResult() - this function will return a pointer to an 
AudioSampleBuffer. Note that from now on, you are responsible for eventual deletion of this buffer
when it's not needed anymore. If runThread() returns false, this means that the rendering process 
was cancelled by the user - in this case, any partially rendered buffer will have been deleted 
internally and getRenderingResult will return a NULL pointer, so in that case, you can just forget
about the buffer.

*/

class AudioRenderingThread : public ThreadWithProgressWindow 
{  

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  AudioRenderingThread();  

  /** Destructor. */
  virtual ~AudioRenderingThread();                             

  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Sets the Arrangement that should be rendered. */
  virtual void setArrangementToRender(Arrangement* newArrangementToRender); 

  /** Sets up the sample rate at which the Arrangement is to be rendered. */
  virtual void setRenderingSampleRate(double newSampleRate);

  //-----------------------------------------------------------------------------------------------
  // inquiry:

  /** Returns the AudioSampleBuffer that resulted from rendering an Arrangement. */
  virtual AudioSampleBuffer* getRenderingResult();

  //-----------------------------------------------------------------------------------------------
  // worker functions:

  /** Does the actual audio-rendering work. */
  virtual void run();

  //===============================================================================================
  juce_UseDebuggingNewOperator;

protected:

  /** This is a pointer to the Arrangement object, that will be rendered into an AudioFileBuffer by 
  this AudioRenderingThread. */
  Arrangement* arrangementToRender;

  /** This is a pointer to the buffer which will eventually contain the result of the rendering 
  process. */
  AudioSampleBuffer* renderingResult;

  double renderingSampleRate;

};

#endif  