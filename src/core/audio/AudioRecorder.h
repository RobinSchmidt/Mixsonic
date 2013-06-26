#ifndef AudioRecorder_h
#define AudioRecorder_h

#include "AudioFileBuffer.h"

/**

This class implements an audio recorder that accepts buffers of audiodata at its input and writes 
them into a file. The incoming audiodata is buffered internally and the writing is done in a 
separate thread.

*/

class AudioRecorder 
{  

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  AudioRecorder();  

  /** Destructor. */
  virtual ~AudioRecorder();                             

  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Sets the file into which the audio data will be recorded. */
  virtual void setRecordingOptions(const File& newTargetFile, double sampleRate, 
    unsigned int numChannels, int bitsPerSample);

  /** Starts recording. */
  virtual void startRecording();

  /** Stops recording. */
  virtual void stopRecording(bool finalizeFile);

  //-----------------------------------------------------------------------------------------------
  // inquiry:

  /** Informs, if the recorder is currently recording or not. */
  virtual bool isRecording() { return recordingFlag; }

  //-----------------------------------------------------------------------------------------------
  // recording:

  /** Records a block of audio samples. If numChannels is greater than the value that was passed to
  setNumChannels, than the additional channels in the passed input buffer will be omitted. If it is 
  smaller, than the additional channels in the recording buffer will be filled with zeros. */
  virtual void recordAudioBlock(const float** audioData, int numSamples, int numChannels);

  //===============================================================================================
  juce_UseDebuggingNewOperator;

protected:

  /** Cleans up dynamically allocated members (i.e. targetFileStream and writer). */
  virtual void cleanUp();

  File               targetFile;
  FileOutputStream*  targetFileStream;
  AudioFormatWriter* writer;
  bool               recordingFlag;

};

#endif  