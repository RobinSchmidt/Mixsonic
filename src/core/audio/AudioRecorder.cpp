#include "AudioRecorder.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction:

AudioRecorder::AudioRecorder() 
{
  writer           = NULL;
  targetFileStream = NULL;
  recordingFlag    = false;
}

AudioRecorder::~AudioRecorder()
{
  stopRecording(true);
  cleanUp();
}

//-------------------------------------------------------------------------------------------------
// setup:

void AudioRecorder::setRecordingOptions(const File& newTargetFile, double sampleRate, 
                                        unsigned int numChannels, int bitsPerSample)
{
  targetFile = newTargetFile;

  AudioFormat* audioFormat = NULL;
  if( targetFile.hasFileExtension(String("wav")) )
    audioFormat = new WavAudioFormat;
  else if( targetFile.hasFileExtension(String("aiff")) )
    audioFormat = new AiffAudioFormat;
  else if( targetFile.hasFileExtension(String("flac")) )
    audioFormat = new FlacAudioFormat;

  if( audioFormat != NULL )
  {
    targetFileStream = targetFile.createOutputStream();
    if( targetFileStream == NULL )
    {
      showFileOutputStreamCreateErrorBox(targetFile);
      delete audioFormat;
      return;
    }

    writer = audioFormat->createWriterFor(targetFileStream, sampleRate, numChannels, bitsPerSample, 
                                          StringPairArray(), 0);
    if( writer == NULL )
    {
      showAudioFormatWriterCreateErrorBox(String("AudioRecorder::setRecordingOptions"));
      delete audioFormat;
      return;
    }

    delete audioFormat;
  }
  else
    cleanUp();
}

void AudioRecorder::startRecording()
{
  recordingFlag = true;
}

void AudioRecorder::stopRecording(bool finalizeFile)
{
  recordingFlag = false;
  if( finalizeFile == true )
    cleanUp();
}

//-------------------------------------------------------------------------------------------------
// recording:

void AudioRecorder::recordAudioBlock(const float** audioData, int numSamples, int numChannels)
{
  if( recordingFlag == false )
    return;

  float** ptr = (float**) audioData;
  AudioSampleBuffer tmpBuffer(ptr, numChannels, numSamples);

  writer->writeFromAudioSampleBuffer(tmpBuffer, 0, numSamples);
}

void AudioRecorder::cleanUp()
{
  if( writer != NULL )
  {
    targetFileStream->flush();
    delete writer;      // the destructor will also delete the targetFileStream
    writer           = NULL;
    targetFileStream = NULL;
  } 
  else if( targetFileStream != NULL )
  {
    // we have no writer-destructor call to automaitcally delete the stream, so we do it ourselves:
    targetFileStream->flush();
    delete targetFileStream;
    targetFileStream = NULL;
  }  
}
