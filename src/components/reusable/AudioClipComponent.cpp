#include "AudioClipComponent.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction and static member intialization:

const double AudioClipComponent::minLevel = -35.0;
const double AudioClipComponent::maxLevel =  12.0;

AudioClipComponent::AudioClipComponent(AudioClip* newClipToEdit) 
: ClipComponent(newClipToEdit), AudioFileBufferUser(newClipToEdit->bufferToUse)
{
  Component::setName(String("AudioClipComponent"));

  ScopedLock pointerLock(audioFileBufferPointerLock);
  ScopedLock sl(clipLock);
  audioClipToEdit = newClipToEdit;
  initialize();
}

AudioClipComponent::AudioClipComponent(const AudioClipComponent &other)
: ClipComponent(NULL), AudioFileBufferUser(other.bufferToUse)
{
  Component::setName(String("CopiedAudioClipComponent"));

  ScopedLock pointerLock(audioFileBufferPointerLock);
  ScopedLock sl(clipLock);

  // when copying and AudioClipComponent, we want to create a new AudioClip for it which has the
  // same data as the AudioClip underneath the other AudioClipComponent
  
  // we may use the copy constructor of AudioClip to create a new AudioClip based on the 
  // other one...:
  audioClipToEdit = new AudioClip(*other.audioClipToEdit); 
  clipToEdit      = audioClipToEdit;

  // ...but we subscribe ourselves as ClipDeletionWatcher:
  if( clipToEdit != NULL )
    clipToEdit->addClipDeletionWatcher(this);

  // \todo: when creating an AudioClipComponent based on another one, we can (and probably should)
  // copy the cached image data for the display
  // conceptually, like: this->waveDisplay->waveDisplayL->clientAreaImage->copyImageData(other->...
  // but it has to be done differently

  initialize();
}

/*
AudioClipComponent::~AudioClipComponent()
{
  //ScopedLock sl(clipLock);
  deleteAllChildren();
}
*/

//-------------------------------------------------------------------------------------------------
// setup:

void AudioClipComponent::assignAudioFileBuffer(AudioFileBuffer* newBuffer)
{
  AudioFileBufferUser::assignAudioFileBuffer(newBuffer);
  audioClipToEdit->assignAudioFileBuffer(newBuffer);
  waveDisplay->assignAudioFileBuffer(newBuffer);

  caption.setText(audioClipToEdit->getFileNameWithoutExtension()); 
  updateDisplay();

  /*
  ScopedLock pointerLock(audioFileBufferPointerLock);
  ScopedLock sl(clipLock);

  AudioFileBufferUser::assignAudioFileBuffer(newBuffer);

  if( audioClipToEdit == NULL )
    return;

  audioClipToEdit->assignAudioFileBuffer(newBuffer);
  waveDisplay->assignAudioFileBuffer(newBuffer);
  caption.setText(audioClipToEdit->getFileNameWithoutExtension(), Font(16));
  updateDisplay();
  */
}

void AudioClipComponent::setTimeRangeInComponent(double newMinTimeInSeconds,                                 
                                                 double newMaxTimeInSeconds)
{
  //ScopedLock pointerLock(audioFileBufferPointerLock);
  ScopedLock sl(clipLock);
  if( audioClipToEdit == NULL )
    return;
  ClipComponent::setTimeRangeInComponent(newMinTimeInSeconds, newMaxTimeInSeconds);
  waveDisplay->setCurrentRangeX(newMinTimeInSeconds, newMaxTimeInSeconds);
}

void AudioClipComponent::setTimeRangeToRender(double newMinTimeInSeconds, 
                                              double newMaxTimeInSeconds)
{
  //ScopedLock pointerLock(audioFileBufferPointerLock);
  ScopedLock sl(clipLock);
  if( audioClipToEdit == NULL )
    return;
  ClipComponent::setTimeRangeToRender(newMinTimeInSeconds, newMaxTimeInSeconds);
  waveDisplay->setVisibleTimeRange(newMinTimeInSeconds, newMaxTimeInSeconds);
}

void AudioClipComponent::deleteUnderlyingClip()
{
  //ScopedLock pointerLock(audioFileBufferPointerLock);
  ScopedLock sl(clipLock);
  ClipComponent::deleteUnderlyingClip();
  audioClipToEdit = NULL;
}

void AudioClipComponent::setDrawingThread(TimeSliceThread *newDrawingThread)
{
  waveDisplay->setDrawingThread(newDrawingThread);
}

//-------------------------------------------------------------------------------------------------
// inquiry:

AudioClip* AudioClipComponent::getUnderlyingAudioClip()
{
  //ScopedLock pointerLock(audioFileBufferPointerLock);
  ScopedLock sl(clipLock);
  return audioClipToEdit;
}

File AudioClipComponent::getUnderlyingFile()
{
  //ScopedLock pointerLock(audioFileBufferPointerLock);
  ScopedLock sl(clipLock);
  if( audioClipToEdit == NULL )
    return File::nonexistent;
  File result = audioClipToEdit->getUnderlyingFile();
  return result;
}

//-------------------------------------------------------------------------------------------------
// callbacks:

void AudioClipComponent::mouseMove(const MouseEvent &e)
{
  MouseEvent e2 = e.getEventRelativeTo(this);
  float x = (float) e2.x;
  float y = (float) e2.y;
  if( isInsideFadeInHandle(x, y) || isInsideFadeOutHandle(x, y) )
    setMouseCursor( MouseCursor(MouseCursor::PointingHandCursor) );
  else if( isOnLevelLine(x, y) )
    setMouseCursor( MouseCursor(MouseCursor::UpDownResizeCursor) );
  else
    ClipComponent::mouseMove(e);
}

void AudioClipComponent::mouseDown(const MouseEvent &e)
{
  ScopedLock sl(clipLock);
  if( audioClipToEdit == NULL )
    return;

  MouseEvent e2 = e.getEventRelativeTo(this);
  float x = (float) e2.x;
  float y = (float) e2.y;
  if( e.mods.isLeftButtonDown() && !e.mods.isShiftDown() )
  {
    if( isInsideFadeInHandle(x, y) )
      currentlyDraggedHandle = FADE_IN_HANDLE;
    else if( isInsideFadeOutHandle(x, y) )
      currentlyDraggedHandle = FADE_OUT_HANDLE;
    else if( isOnLevelLine(x, y) )
      currentlyDraggedHandle = LEVEL_LINE;
    else
    {
      ClipComponent::mouseDown(e);
      return;
    }
  }
  else
  {
    ClipComponent::mouseDown(e);
    return;
  }

  updateDescriptionField();
}

void AudioClipComponent::mouseDrag(const MouseEvent &e)
{
  //ScopedLock pointerLock(audioFileBufferPointerLock);
  ScopedLock sl(clipLock);
  if( audioClipToEdit == NULL )
    return;
  MouseEvent e2 = e.getEventRelativeTo(this);
  
  float x = (float) e2.x;
  if( currentlyDraggedHandle == LEFT_BORDER )
    x = jmin(x, (float) getWidth());
  else if( currentlyDraggedHandle == RIGHT_BORDER )
    x = jmax(x, 0.f);
  else
    x = jlimit(0.f, (float) getWidth(), x);
  
  float y      = jlimit(0.f, (float) getHeight(), (float) e2.y);
  float yDummy = 0.0;

  waveDisplay->waveDisplayL->transformFromComponentsCoordinates(x, yDummy);
  double level = yToLevel(y);

  if( currentlyDraggedHandle == FADE_IN_HANDLE )
  {
    double xWithOffset = x-audioClipToEdit->getActiveRegionStart(TimeUnitConverter::SECONDS, true);
    audioClipToEdit->setFadeInInSeconds(xWithOffset); 
    audioClipToEdit->setLevel(level);
    repaint();
  }
  else if( currentlyDraggedHandle == FADE_OUT_HANDLE )
  {
    audioClipToEdit->setFadeOutInSeconds(
      audioClipToEdit->getActiveRegionEnd(TimeUnitConverter::SECONDS, true)-x ); 
    audioClipToEdit->setLevel(level);
    repaint();
  }
  else if( currentlyDraggedHandle == LEVEL_LINE )
  {
    audioClipToEdit->setLevel(level);
    repaint();
  }
  else if( currentlyDraggedHandle == LEFT_BORDER )
  {
    x = jlimit(0.f, (float) (audioClipToEdit->getActiveRegionEnd(TimeUnitConverter::SECONDS, true)-0.01), x);
    audioClipToEdit->setActiveRegionStart(x, TimeUnitConverter::SECONDS, true);  
    waveDisplay->setCurrentRangeMinX(x);
    sendChangeMessage();
  }
  else if( currentlyDraggedHandle == RIGHT_BORDER )
  {
    x = jlimit((float) (audioClipToEdit->getActiveRegionStart(TimeUnitConverter::SECONDS, true)+0.01),
               (float) (audioClipToEdit->getTotalDuration(TimeUnitConverter::SECONDS)), x);
    audioClipToEdit->setActiveRegionEnd(x, TimeUnitConverter::SECONDS, true);    
    waveDisplay->setCurrentRangeMaxX(x);
    sendChangeMessage();
  }

  updateDescriptionField();
}

void AudioClipComponent::clipIsToBeDeleted(Clip *clipToBeDeleted)
{
  //ScopedLock pointerLock(audioFileBufferPointerLock);
  ScopedLock sl(clipLock);
  if( clipToBeDeleted == audioClipToEdit )
  {
    waveDisplay->assignAudioFileBuffer(NULL);
    audioClipToEdit = NULL;
  }
  ClipComponent::clipIsToBeDeleted(clipToBeDeleted);
}

//-------------------------------------------------------------------------------------------------
// appearance stuff:

void AudioClipComponent::paint(Graphics &g)
{
  //g.fillAll(Colours::red); // not necessary to do anything
}

void AudioClipComponent::paintOverChildren(Graphics &g)
{
  ScopedLock sl(clipLock);
  if( audioClipToEdit == NULL )
    return;

  // draw the fade-in/out envelope:
  float r  = (float) handleRadius; 
  float x1 = 0.f;
  float y1 = getEnvelopeAtLeftY();
  float x4 = (float) getWidth();
  float y4 = getEnvelopeAtRightY();
  float x2, y2, x3, y3;
  getFadeInHandleCenter(x2, y2);
  getFadeOutHandleCenter(x3, y3);
  //g.setColour(Skin::getInstance()->markerColor.withAlpha(0.75f));
  //g.setColour(skin->handleColor.withAlpha(0.75f));
  g.setColour(skin->foregroundColor.withAlpha(0.75f)); // maybe get rid of the withAlpha - this can
                                                       // be defined in the xml itself
  g.drawLine(x1, y1, x2, y2, 2.f);
  g.drawLine(x2, y2, x3, y3, 2.f);
  g.drawLine(x3, y3, x4, y4, 2.f);
  g.fillEllipse(x2-r, y2-r, 2.f*r, 2.f*r);
  g.fillEllipse(x3-r, y3-r, 2.f*r, 2.f*r);

  ClipComponent::paintOverChildren(g);
}

void AudioClipComponent::resized()
{
  //ScopedLock pointerLock(audioFileBufferPointerLock);
  ScopedLock sl(clipLock);
  if( audioClipToEdit == NULL )
    return;

  waveDisplay->setBounds(0, 0, getWidth(), getHeight());
  ClipComponent::resized();
}

void AudioClipComponent::setMouseCursor(const MouseCursor &cursorType)
{
  waveDisplay->waveDisplayL->setMouseCursor(cursorType);  
  waveDisplay->waveDisplayR->setMouseCursor(cursorType);
}

//-------------------------------------------------------------------------------------------------
// internal functions:

void AudioClipComponent::initialize()
{
  ScopedLock lockForAudioClip(clipLock);
  ScopedLock pointerLock1(audioFileBufferPointerLock);
  ScopedLock pointerLock2(audioClipToEdit->audioFileBufferPointerLock);

  if( audioClipToEdit == NULL )
    waveDisplay = new DualWaveformDisplay(NULL);
  else
    waveDisplay = new DualWaveformDisplay(audioClipToEdit->getUsedBuffer());

  waveDisplay->setBackgroundColor(skin->backgroundColor);
  waveDisplay->setGraphColor(     skin->middlegroundColor);
  waveDisplay->setOutlineColor(   skin->outlineColor);

  addAndMakeVisible( waveDisplay  );
  waveDisplay->setMaximumRangeY(-1.1, 1.1);
  waveDisplay->setCurrentRangeY(-1.1, 1.1);
  waveDisplay->addMouseListener(this, true);

  /*
  \todo check, what this was - maybe remove
  //waveDisplay = new DualWaveformDisplay(audioClipToEdit->getUsedBuffer());
  if( audioClipToEdit != NULL )
  {
    //ScopedLock pointerLock(audioClipToEdit->audioFileBufferPointerLock);
    //waveDisplay->assignAudioFileBuffer(audioClipToEdit->getUsedBuffer());
  }
  */

  ClipComponent::initialize();
}

void AudioClipComponent::updateDisplay()
{
  ScopedLock sl(clipLock);
  if( audioClipToEdit == NULL )
    return;

  //ScopedLock pointerLock(audioFileBufferPointerLock);


  if( audioClipToEdit->getNumSamples() < 1 )
  {
    // \todo: maybe clear the displays...
    return;
  }

  waveDisplay->setMaximumRangeX(0.0, audioClipToEdit->getBufferLengthInSeconds() );
  waveDisplay->setCurrentRangeX(0.0, audioClipToEdit->getBufferLengthInSeconds() );
  caption.setText(audioClipToEdit->getFileNameWithoutExtension());
  int w = getWidth();
  int h = getHeight();

  waveDisplay->setBounds(0, 0, w, h);
  waveDisplay->updatePlotImage();  // \todo: check if this is necesarry
  //waveDisplay->assignAudioFileBuffer(audioClipToEdit->bufferToUse);
}

void AudioClipComponent::updateDescriptionField()
{
  ScopedLock sl(clipLock);
  if( audioClipToEdit == NULL )
    return;

  if( descriptionField != NULL )
    descriptionField->setText(audioClipToEdit->getLevelAndFadeString(), false);
}

float AudioClipComponent::levelToY(float level)
{
  float h  = (float) getHeight();
  float y1 = (float) mapLinearToLinear(level, minLevel, maxLevel, 0.0, h);
  return h-y1;
}

float AudioClipComponent::yToLevel(float y)
{
  float h  = (float) getHeight();
  float y1 = h-y;
  return (float) mapLinearToLinear(y1, 0.0, h, minLevel, maxLevel);
}

float AudioClipComponent::getFadeHandleY()
{
  ScopedLock sl(clipLock);
  if( audioClipToEdit == NULL )
    0.f;
  return levelToY( (float) audioClipToEdit->getLevel() );
}

void AudioClipComponent::getFadeInHandleCenter(float &x, float &y)
{
  ScopedLock sl(clipLock);
  if( audioClipToEdit == NULL )
    return;
  x = (float) (audioClipToEdit->getActiveRegionStart(TimeUnitConverter::SECONDS, true) 
               + audioClipToEdit->getFadeInInSeconds() );
  y = 0.0; 
  waveDisplay->waveDisplayL->transformToComponentsCoordinates(x, y);
  y = getFadeHandleY();
}

void AudioClipComponent::getFadeOutHandleCenter(float &x, float &y)
{
  ScopedLock sl(clipLock);
  if( audioClipToEdit == NULL )
    return;
  x = (float) (audioClipToEdit->getActiveRegionEnd(TimeUnitConverter::SECONDS, true) 
               - audioClipToEdit->getFadeOutInSeconds() );
  y = 0.0;
  waveDisplay->waveDisplayL->transformToComponentsCoordinates(x, y);
  y = getFadeHandleY();
}

bool AudioClipComponent::isInsideFadeInHandle(float x, float y)
{
  float xh, yh;
  getFadeInHandleCenter(xh, yh);
  float d = sqrt( (x-xh)*(x-xh) + (y-yh)*(y-yh) ); // distance of (x,y) from the handle's center
  if( d <= handleRadius )
    return true;
  else
    return false;
}

bool AudioClipComponent::isInsideFadeOutHandle(float x, float y)
{
  float xh, yh;
  getFadeOutHandleCenter(xh, yh);
  float d = sqrt( (x-xh)*(x-xh) + (y-yh)*(y-yh) ); // distance of (x,y) from the handles center
  if( d <= handleRadius )
    return true;
  else
    return false;
}

bool AudioClipComponent::isOnLevelLine(float x, float y)
{
  float xIn, xOut, yIn, yOut;
  getFadeInHandleCenter(xIn, yIn);
  getFadeOutHandleCenter(xOut, yOut);
  if( x <= xIn || x >= xOut )
    return false;

  // x is now assured to be in between the fade handles...
  float d = fabs(y-yIn);
  if( d <= 2 )
    return true;
  else
    return false;
}

bool AudioClipComponent::isOnMuteButton(float x, float y)
{
  if( isOnLevelLine(x,y) )
    return false;    // level-line overrides mute-button
  else
    return ClipComponent::isOnMuteButton(x, y);
}

bool AudioClipComponent::isOverLeftBorder(float x, float y)
{  
  if( isInsideFadeInHandle(x, y) )
    return false;    // fade handle overrides border
  else
    return ClipComponent::isOverLeftBorder(x, y);
}

bool AudioClipComponent::isOverRightBorder(float x, float y)
{
  if( isInsideFadeOutHandle(x, y) )
    return false;    // fade handle overrides border
  else
    return ClipComponent::isOverRightBorder(x, y);
}

float AudioClipComponent::getEnvelopeAtLeftY()
{
  //ScopedLock pointerLock(audioFileBufferPointerLock);
  ScopedLock sl(clipLock);
  if( audioClipToEdit == NULL )
    return 0.f;

  double tl    = waveDisplay->getCurrentRangeMinX();
  double start = audioClipToEdit->getActiveRegionStart(TimeUnitConverter::SECONDS, true);
  double factor;
  if( tl <= start )
    factor = 0.0;
  else if( tl >= start + audioClipToEdit->getFadeInInSeconds() )
    factor = 1.0;
  else
    factor = (tl-start) / audioClipToEdit->getFadeInInSeconds();

  double level = audioClipToEdit->getLevel();
  double y1    = factor * mapLinearToLinear(level, minLevel, maxLevel, 0.0, getHeight());
  return (float) (getHeight() - y1);
}

float AudioClipComponent::getEnvelopeAtRightY()
{
  //ScopedLock pointerLock(audioFileBufferPointerLock);
  ScopedLock sl(clipLock);
  if( audioClipToEdit == NULL )
    return 0.f;

  double tr        = waveDisplay->getCurrentRangeMaxX();
  double length    = audioClipToEdit->getActiveRegionEnd(TimeUnitConverter::SECONDS, true);
  double fadeOut   = audioClipToEdit->getFadeOutInSeconds();
  double fadeStart = length-fadeOut;
  double factor;
  if( tr >= length )
    factor = 0.0;
  else if( tr <= fadeStart )
    factor = 1.0;
  else
    factor = 1.0 - (tr-fadeStart) / fadeOut;

  double level = audioClipToEdit->getLevel();
  double y1    = factor * mapLinearToLinear(level, minLevel, maxLevel, 0.0, getHeight());
  return (float) (getHeight() - y1);
}

//-----------------------------------------------------------------------------------------------
// thread synchronization:

void AudioClipComponent::lockClipAndBufferPointers()
{
  clipLock.enter();
  AudioFileBufferUser::lockUsedBufferPointer();

  if( audioClipToEdit != NULL )
    audioClipToEdit->lockUsedBufferPointer();
  else
    jassertfalse;

  if( waveDisplay != NULL )
    waveDisplay->lockUsedBufferPointer();
  else
    jassertfalse;
}

void AudioClipComponent::unlockClipAndBufferPointers()
{
  if( waveDisplay != NULL )
    waveDisplay->unlockUsedBufferPointer();
  else
    jassertfalse;

  if( audioClipToEdit != NULL )
    audioClipToEdit->unlockUsedBufferPointer();
  else
    jassertfalse;

  AudioFileBufferUser::unlockUsedBufferPointer();
  clipLock.exit();
}

/*
void AudioClipComponent::acquireWriteLocks()
{
  clipLock.enter();
  acquireAudioFileBufferWriteLock();

  if( audioClipToEdit != NULL )
    audioClipToEdit->acquireAudioFileBufferWriteLock();

  if( waveDisplay != NULL )
    waveDisplay->acquireAudioFileBufferWriteLock();
}

void AudioClipComponent::releaseWriteLocks()
{
  if( waveDisplay != NULL )
    waveDisplay->releaseAudioFileBufferWriteLock();

  if( audioClipToEdit != NULL )
    audioClipToEdit->releaseAudioFileBufferWriteLock();

  releaseAudioFileBufferWriteLock();
  clipLock.exit();
}
*/

//-----------------------------------------------------------------------------------------------
// delegations to the underlying AudioClip:

void AudioClipComponent::setFadeInInSeconds(double newFadeInInSeconds)
{
  ScopedLock sl(clipLock);
  if( audioClipToEdit != NULL )
  {
    audioClipToEdit->setFadeInInSeconds(newFadeInInSeconds);
    repaint();
  }
}

void AudioClipComponent::setFadeOutInSeconds(double newFadeOutInSeconds)
{
  ScopedLock sl(clipLock);
  if( audioClipToEdit != NULL )
  {
    audioClipToEdit->setFadeOutInSeconds(newFadeOutInSeconds);
    repaint();
  }
}