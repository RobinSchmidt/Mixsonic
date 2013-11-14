#include "MixsonicBrowserBase.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction:

MixsonicBrowserBase::MixsonicBrowserBase(const File &directoryToUse) 
: Component(String("MixsonicBrowserBase")), 
  browserUpdateThread(String("MixsonicBrowserUpdateThread")), 
  ResamplingAudioSource(&inputSource, false)
{
  // create the headline label:
  addAndMakeVisible( headline = new RHeadlineLabel(String("HeadlineLabel"), String("BrowserBase")) );
  //headline->setFont(Font(20, Font::bold));
  headline->setJustificationType(Justification::centred);

  // create the preview buttons:
  addAndMakeVisible( pauseButton = new MixsonicButton() );
  pauseButton->setDescription(browserPauseButtonStr);
  pauseButton->setClickingTogglesState(true);
  pauseButton->addListener(this);

  addAndMakeVisible( playButton = new MixsonicButton() );
  playButton->setDescription(browserPlayButtonStr);
  playButton->setClickingTogglesState(true);
  playButton->addListener(this);

  addAndMakeVisible( rewindButton = new MixsonicButton() );
  rewindButton->setDescription(browserRewindButtonStr);
  rewindButton->setClickingTogglesState(false);
  rewindButton->addListener(this);
	
  browserUpdateThread.startThread (5);	
}

MixsonicBrowserBase::~MixsonicBrowserBase()
{
  deleteAllChildren();
  browserUpdateThread.stopThread (2000);
}

//-------------------------------------------------------------------------------------------------
// setup:

void MixsonicBrowserBase::setDescriptionField(Label* newDescriptionField)
{
  pauseButton->setDescriptionField(newDescriptionField);  
  playButton->setDescriptionField(newDescriptionField);  
  rewindButton->setDescriptionField(newDescriptionField);  
}

//-------------------------------------------------------------------------------------------------
// callbacks:

void MixsonicBrowserBase::buttonClicked(Button *buttonThatWasClicked)
{
  if( buttonThatWasClicked == pauseButton )
    pausePlayback( pauseButton->getToggleState() );
  if( buttonThatWasClicked == rewindButton )
    restartPlayback();
  /*
  // \todo this should now be handled by the MixsonicContentComponent:
  else if( buttonThatWasClicked == playButton )
  {
    if( playButton->getToggleState() == true )
      startPlayback( getSelectedFile() );
    else
      stopPlayback();
  }
  */
}

void MixsonicBrowserBase::resized()
{
  int x = 0;
  int y = 0;
  int w = getWidth();
  //int h = getHeight();

  headline->setBounds(0, 0, w, 28);

  x = w/2-12;
  y = getHeight()-27;
  pauseButton->setBounds(x, y, 24, 24);
  x = pauseButton->getX()-24-12;
  playButton->setBounds(x, y, 24, 24);
  x = pauseButton->getX()+24+12;
  rewindButton->setBounds(x, y, 24, 24);

  /*
  x = 3;
  y = 30;
  h = getHeight()-2*30;
  w = getWidth()-x-3;
  audioFileBrowser->setBounds(x, y, w, h);
  */
}

//-------------------------------------------------------------------------------------------------
// others:

void MixsonicBrowserBase::startPlaybackIfDesired(const File &fileToPlay)
{
  if( playButton->getToggleState() && !pauseButton->getToggleState() )
    startPlayback(fileToPlay);
  else
    stopPlayback();
}

void MixsonicBrowserBase::startPlayback(const File &fileToPlay)
{
  audioBuffer.loadAudioDataFromFile(fileToPlay);

  if( audioBuffer.isAudioFileValid() )
  {
    // setup the inherited ResamplingAudioSource object:
    ResamplingAudioSource::setResamplingRatio(
      audioBuffer.getFileSampleRate()/TimeUnitConverter::getSystemSampleRate());
    ResamplingAudioSource::prepareToPlay(960, TimeUnitConverter::getSystemSampleRate());

    // start playback immediately when the playButton is already down:
    if( playButton->getToggleState() == true )
      inputSource.startPlayback(&audioBuffer);
  }
  else
    stopPlayback();
}

void MixsonicBrowserBase::stopPlayback()
{
  // create an empty dummy buffer and 'play' it - this will actually stop any playback which is
  // currently running:
  AudioSampleBuffer* dummyBuffer = new AudioSampleBuffer(1, 0);
  dummyBuffer->clear();  
  inputSource.startPlayback(dummyBuffer);
  delete dummyBuffer;
}

void MixsonicBrowserBase::pausePlayback(bool shouldBePaused)
{
  inputSource.pausePlayback(shouldBePaused);
}

void MixsonicBrowserBase::restartPlayback()
{
  inputSource.restartPlayback();
}