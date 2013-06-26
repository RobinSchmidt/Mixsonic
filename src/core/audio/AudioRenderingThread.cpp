#include "AudioRenderingThread.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction:

AudioRenderingThread::AudioRenderingThread() 
: ThreadWithProgressWindow(String("Rendering Audio"), true, true)
{
  arrangementToRender = NULL;
  renderingResult     = NULL;
  renderingSampleRate = 44100.0;
}

AudioRenderingThread::~AudioRenderingThread()
{

}
  
//-------------------------------------------------------------------------------------------------
// setup:

void AudioRenderingThread::setArrangementToRender(Arrangement *newArrangementToRender)
{
  arrangementToRender = newArrangementToRender;
}

void AudioRenderingThread::setRenderingSampleRate(double newSampleRate)
{
  jassert(newSampleRate > 0.0); // cannot render audio at zero or negative samplerates
  if( newSampleRate > 0.0 )
    renderingSampleRate = newSampleRate;
}

//-------------------------------------------------------------------------------------------------
// inquiry:

AudioSampleBuffer* AudioRenderingThread::getRenderingResult()
{
  return renderingResult;
}

//-------------------------------------------------------------------------------------------------
// worker functions:

void AudioRenderingThread::run()
{
  if( arrangementToRender == NULL )
    return;

  // define some blocksize for rendering:
  int renderingBlockSize = 1024;

  // aquire the mutex-lock for the arrangement and prepare the arrangement for (non-realtime) 
  // playback:
  arrangementToRender->lockArrangement();
  arrangementToRender->prepareToPlay(renderingBlockSize, renderingSampleRate);
  arrangementToRender->setNextReadPosition(0);

  // get the length of the arrangement (in samples) and calculate the number of blocks to render:
  int numSamplesToRender = (int) jmin(arrangementToRender->getTotalLength(), (int64) INT_MAX); 
  int numBlocksToRender  = (int) ceil( (double) numSamplesToRender / (double) renderingBlockSize );

  // allocate memory for the rendering result and check for success:
  renderingResult = new AudioSampleBuffer(2, numSamplesToRender);
  if( renderingResult == NULL )
  {
    showMemoryAllocationErrorBox(String("AudioRenderingThread::run"));
    arrangementToRender->unlockArrangement();
    return;
  }

  // we need to wrap the AudioSampleBuffer into the AudioSourceChannelInfo struct, to make it 
  // suitable for the getNextAudioBlock method of AudioSource:
  AudioSourceChannelInfo channelInfo;
  channelInfo.buffer      = renderingResult;
  channelInfo.numSamples  = renderingBlockSize;
  channelInfo.startSample = 0;

  // now we are set up and start the actual rendering:
  arrangementToRender->play(true);
  for(int b=0; b<numBlocksToRender-1; b++) 
  {
    // this is how we know if the user's pressed 'cancel':
    if( threadShouldExit() )
    {
      // do some cleaning up (delete the partially rendered buffer):
      delete renderingResult;
      renderingResult = NULL;
      break;
    }

    // this will update the progress bar on the dialog box:
    double progress = (double) b / (double) numBlocksToRender;
    setProgress(progress);

    // fill the channelInfo.buffer block by block:
    channelInfo.startSample = b*renderingBlockSize;
    arrangementToRender->getNextAudioBlock(channelInfo);
  }

  // the loop has rendered only those blocks which have the full renderingBlockSize - the last
  // block may be shorter than that, so we render it seperately:
  int remainingSamples = numSamplesToRender - (numBlocksToRender-1)*renderingBlockSize;
  if( remainingSamples > 0 )
  {
    channelInfo.startSample = (numBlocksToRender-1)*renderingBlockSize;
    channelInfo.numSamples  = remainingSamples;
    arrangementToRender->getNextAudioBlock(channelInfo);
  }

  // normalize the result:
  normalizeAudioSampleBuffer(renderingResult, 1.0);

  // we are through - stop playing and release the mutex lock:   
  arrangementToRender->play(false);
  arrangementToRender->unlockArrangement();
}



