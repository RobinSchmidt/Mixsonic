#include "Arrangement.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction:

Arrangement::Arrangement(SamplePool *samplePoolToUse) : SamplePoolClient(samplePoolToUse)
{
  clipSelectionListener = NULL;
  defaultSkipInSeconds  = 5.0;
  initializeArrangement();
}

Arrangement::~Arrangement()
{
  // we must make sure, that all clips are destroyed *before* the samplePool gets destroyed:
  tracks.getLock().enter();
  for(int t=0; t<tracks.size(); t++)
    tracks[t]->removeAllClips(true);
  tracks.getLock().exit();

  for(int c=0; c<clipsInClipboard.size(); c++)
    delete clipsInClipboard[c];
}

//-------------------------------------------------------------------------------------------------
// setup:

void Arrangement::initializeArrangement()
{
  lockArrangement();

  //lengthInBeats         = 180.0;
  setMaxTimeInBeats(180.0);
  masterVolumeLevel     = 0.0;
  masterAmplitudeFactor = 1.f;
  globalTempoInBpm      = 120.0;
  systemSampleRate      = 44100.0;
  positionInSamples     = 0;
  soloedTrack           = -1;
  isPlaying             = false;

  clearClipBoard();

  // aquire mutex-locks for the sample-pool and the array of tracks:
  samplePool->lockAudioFileBufferArray();
  tracks.getLock().enter();

  // clear the array of tracks (this is an OwnedArray, so it will also delete the Track-objects, 
  // and Track's destructor will also delete the contained Clip objects):
  tracks.clear();

  // clear also the samplePool:
  samplePool->clear();

  // add some new tracks \todo: have a function addTrack - maybe with name and index as parameter
  Track* track;
  for(int t=0; t<numTracks; t++)
  {
    track = new Track(t);
    track->setName(String("Track ") + String(t+1));
    track->registerObserver(this);
    tracks.add(track);
  }

  // release the aquired mutex-locks for the arrays:
  samplePool->unlockAudioFileBufferArray();
  tracks.getLock().exit();
  unlockArrangement();
}
/*
void Arrangement::setTrackSolo(int trackToBeSoloed)
{
  lockArrangement();
  tracks.getLock().enter();

  if( trackToBeSoloed >= 0 && trackToBeSoloed < tracks.size() )
    soloedTrack = trackToBeSoloed;
  else
    soloedTrack = -1;

  for(int t=0; t<tracks.size(); t++)
  {
    if( t == soloedTrack )
      tracks[t]->setSolo(true);
    else
      tracks[t]->setSolo(false);
  }

  tracks.getLock().exit();
  unlockArrangement();
}
*/
//-------------------------------------------------------------------------------------------------
// inquiry:

double Arrangement::getMasterVolumeLevel() const
{
  return masterVolumeLevel;
}

double Arrangement::getCurrentTimeInBeats() const
{
  double timeInSeconds = (double) getNextReadPosition() / TimeUnitConverter::systemSampleRate;
  return secondsToBeats(timeInSeconds);
}

double Arrangement::getCurrentTimeInSeconds() const
{
  int64 timeInSamples = getNextReadPosition();
  return timeInSamples / TimeUnitConverter::systemSampleRate;
}

double Arrangement::getLastClipEndTimeInBeats(bool onlySelectedClips)
{
  lockArrangement();
  tracks.getLock().enter();

  double end;
  double result = 0.0;
  Track  *track;
  int    t, c;
  if( onlySelectedClips == true )
  {
    for(t=0; t<tracks.size(); t++)
    {
      track = tracks[t];
      for(c=0; c<track->getNumClips(); c++)
      {
        if( track->isClipSelected(c) ) // consider only the selected clips
        {
          end = track->audioClips[c]->getActiveRegionEnd(BEATS, false);
          if( end > result )
            result = end;
        }
      }
    }
  }
  else
  {
    for(t=0; t<tracks.size(); t++)
    {
      track = tracks[t];
      for(c=0; c<track->getNumClips(); c++)
      {
        end = track->audioClips[c]->getActiveRegionEnd(BEATS, false);
        if( end > result )
          result = end;
      }
    }
  }

  tracks.getLock().exit();
  unlockArrangement();
  return result;
}

//-------------------------------------------------------------------------------------------------
// communication with other classes:

void Arrangement::registerSelectionListener(SelectionListener *newListener)
{
  clipSelectionListener = newListener;
}

//-------------------------------------------------------------------------------------------------
// callbacks:

void Arrangement::samplePathHasChanged(int index, const File& oldFilePath, 
                                       const File& newFilePath)
{
  // need to do something here?
}

void Arrangement::sampleIsToBeDeleted(int index)
{
  lockArrangement();
  tracks.getLock().enter();
  samplePool->lockAudioFileBufferArray();
  for(int t = 0; t < tracks.size(); t++)
    tracks[t]->removeAllClipsUsingBuffer(samplePool->getAudioFileBuffer(index), true);
  samplePool->unlockAudioFileBufferArray();
  tracks.getLock().exit();
  unlockArrangement();
}

void Arrangement::trackSoloStateChanged(Track *track, bool isSolo)
{
  lockArrangement();  
  tracks.getLock().enter();
  if( isSolo )
  {
    // switching solo on for one track must switch solo off for all other tracks:
    jassert( tracks.contains(track) );
    for(int t = 0; t < tracks.size(); t++)
    {
      if( tracks[t] != track )
        tracks[t]->setSolo(false);
    }
  }
  updateSoloedTrack();
  tracks.getLock().exit();
  unlockArrangement();
}

//-------------------------------------------------------------------------------------------------
// add and remove clips:

void Arrangement::deSelectAllClips()
{
  lockArrangement();
  tracks.getLock().enter();
  for(int t=0; t<tracks.size(); t++)
    tracks[t]->deSelectAllClips();
  tracks.getLock().exit();
  unlockArrangement();
}

bool Arrangement::addClip(AudioClip* clipToAdd, int trackIndex)
{
  lockArrangement();
  tracks.getLock().enter();

  if( trackIndex < 0 || trackIndex >= tracks.size() )
  {
    tracks.getLock().exit();
    unlockArrangement();
    return false;          // trackIndex was out of range
  }

  // add the clip to one of the tracks:
  tracks[trackIndex]->addClip(clipToAdd);

  clipToAdd->lockUsedBufferPointer();
  samplePool->lockAudioFileBufferArray();

  int indexInPool = samplePool->addIfNotAlreadyThere(clipToAdd->getUsedBuffer());
  clipToAdd->assignAudioFileBuffer(samplePool->getAudioFileBuffer(indexInPool));

  samplePool->unlockAudioFileBufferArray();
  clipToAdd->unlockUsedBufferPointer();

  tracks.getLock().exit();
  unlockArrangement();

  return true;
}

void Arrangement::cutSelectedClipsIntoClipboard(bool forgetOldClipboardContents)
{
  copySelectedClipsIntoClipboard(forgetOldClipboardContents);
  deleteSelectedClips();
  // this works but maybe we could better without copying and deleting by simply moving the 
  // existing ones into the clipboard
}

void Arrangement::copySelectedClipsIntoClipboard(bool forgetOldClipboardContents)
{
  lockArrangement();
  clipsInClipboard.getLock().enter();
  tracks.getLock().enter();

  // some loacal variables that will be used subsequently:
  int t, c;
  Track     *track;
  AudioClip *clipCopy;

  // if so chosen, delete all the temporarily stored clips in the clipboard and clear the array
  // which maintains the pointers to them:
  if( forgetOldClipboardContents == true )
    clearClipBoard();

  // for each clip which is selected, create a temporary copy and put it into the clipboard:
  for(t=0; t<tracks.size(); t++)
  {
    track = tracks[t];
    for(c=0; c<track->getNumClips(); c++)
    {
      if( track->isClipSelected(c) )
      {
        clipCopy = track->getClipCopy(c);
        clipCopy->removeAllClipDeletionWatchers();
        clipsInClipboard.add(clipCopy);
      }
    }
  }

  tracks.getLock().exit();
  clipsInClipboard.getLock().exit();
  unlockArrangement();
}

void Arrangement::pasteClipsFromClipboard(double timeInBeats)
{
  lockArrangement();
  clipsInClipboard.getLock().enter();
  tracks.getLock().enter();

  deSelectAllClips();

  // find the leftmost clip:
  double minPosition = DBL_MAX;
  int    minIndex    = -1;
  int c;
  AudioClip *clip;
  for(c=0; c<clipsInClipboard.size(); c++)
  {
    clip = clipsInClipboard[c];
    if( clip->getActiveRegionStart(BEATS, false) < minPosition )
    {
      minPosition = clip->getActiveRegionStart(BEATS, false);
      minIndex    = c;
    }
  }

  // create new clips and add them to the tracks (use the same tracks from which they originate) 
  // and adjust their positions along the timeline:
  if( timeInBeats == -1.0 )
    timeInBeats = getCurrentTimeInBeats();
  int t;
  for(c=0; c<clipsInClipboard.size(); c++)
  {
    clip = new AudioClip( *clipsInClipboard[c] ); // copy constructor
    t    = clip->getHostingTrackIndex();
    clip->setClipPosition( timeInBeats + (clip->getClipPosition(BEATS)-minPosition), BEATS );
    clip->prepareToPlay(systemSampleRate);
    if( t >= 0 && t < tracks.size() )
      tracks[t]->addClip(clip);
  }

  tracks.getLock().exit();
  clipsInClipboard.getLock().exit();
  unlockArrangement();
}

void Arrangement::deleteSelectedClips()
{
  lockArrangement();
  tracks.getLock().enter();
  for(int t=0; t<tracks.size(); t++)
    tracks[t]->deleteSelectedClips();
  tracks.getLock().exit();
  unlockArrangement();
}

//-------------------------------------------------------------------------------------------------
// navigate in the arrangement:

void Arrangement::rewindToStart()
{
  lockArrangement();
  setCurrentTimeInSeconds(0.0);
  setNextReadPosition(0);
  unlockArrangement();
}

void Arrangement::setCurrentTimeInBeats(double newCurrentTimeInBeats)
{
  lockArrangement();
  TimeFrame::setCurrentTimeInBeats(newCurrentTimeInBeats);
  setNextReadPosition( (int) (TimeFrame::getCurrentTimeInSeconds()*getSystemSampleRate()) );
  unlockArrangement();
}

void Arrangement::setCurrentTimeInSeconds(double newCurrentTimeInSeconds)
{
  lockArrangement();
  TimeFrame::setCurrentTimeInSeconds(newCurrentTimeInSeconds);
  setNextReadPosition( (int) (TimeFrame::getCurrentTimeInSeconds()*getSystemSampleRate()) );
  unlockArrangement();
}

//-------------------------------------------------------------------------------------------------
// inquiry:

int Arrangement::getNumTracks() const
{
  return numTracks;
}

String Arrangement::getCurrentTimeAsString() const
{
  if( selectedTimeUnit == BEATS )
    return String( getCurrentTimeInBeats() ); // preliminary
  else
    return secondsToString(getCurrentTimeInSeconds()); 
}

//-------------------------------------------------------------------------------------------------
// audio related functions:

void Arrangement::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
  lockArrangement();
  tracks.getLock().enter();
  for(int t=0; t<tracks.size(); t++)
    tracks[t]->prepareToPlay(sampleRate);
  tracks.getLock().exit();
  unlockArrangement();

  // later, when we have multicore support, this is the place where to set up the child 
  // audio-threads
}

void Arrangement::play(bool shouldPlay)
{
  lockArrangement();
  isPlaying = shouldPlay;
  unlockArrangement();
}

void Arrangement::setMasterVolumeLevel(double newMasterVolumeLevel)
{
  lockArrangement();
  masterVolumeLevel     = newMasterVolumeLevel;
  masterAmplitudeFactor = (float) dB2amp(masterVolumeLevel);
  unlockArrangement();
}

void Arrangement::releaseResources()
{

}

void Arrangement::getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill)
{
  bufferToFill.clearActiveBufferRegion();

  if( isPlaying == false )
    return;

  lockArrangement();
  tracks.getLock().enter();

  if( soloedTrack >= 0 && soloedTrack < tracks.size() )
  {
    // just read the signal from the soloed track:
    tracks[soloedTrack]->addSignalToAudioBlock(bufferToFill, positionInSamples);
  }
  else
  {
    // accumulate the signals from all tracks:
    for(int t=0; t<tracks.size(); t++)
      tracks[t]->addSignalToAudioBlock(bufferToFill, positionInSamples);
  }

  // apply the master gain factor:
  bufferToFill.buffer->applyGain(bufferToFill.startSample, bufferToFill.numSamples, 
    masterAmplitudeFactor);

  // increment our sample-counter:
  positionInSamples += bufferToFill.numSamples;

  tracks.getLock().exit();
  unlockArrangement();
}

void Arrangement::setNextReadPosition(int64 newPosition)
{
  lockArrangement();
  positionInSamples = newPosition;
  unlockArrangement();
}

int64 Arrangement::getNextReadPosition() const
{
  return positionInSamples;
}

int64 Arrangement::getTotalLength() const
{
  return (int) ceil(getSystemSampleRate() * getMaxTimeInSeconds());
}

bool Arrangement::isLooping() const
{
  return false;
}

//-------------------------------------------------------------------------------------------------
// state saving and recall:

XmlElement* Arrangement::getStateAsXml(const File& songDirectory) const
{
  XmlElement* xmlState = new XmlElement(String("ARRANGEMENT"));

  //xmlState->setAttribute("songTitle",          songTitle);
  xmlState->setAttribute("songLengthInBeats",  getMaxTimeInBeats());
  xmlState->setAttribute("masterVolumeLevel",  masterVolumeLevel);
  xmlState->setAttribute("tempo",              globalTempoInBpm);
  xmlState->setAttribute("sampleRate",         systemSampleRate);

  // create a child element for each track and add it to the state-element:
  tracks.getLock().enter();
  for(int t=0; t<tracks.size(); t++)
  {
    XmlElement* trackState = tracks[t]->getStateAsXml(songDirectory);
    xmlState->addChildElement(trackState);
  }
  tracks.getLock().exit();

  return xmlState;
}

void Arrangement::setStateFromXml(const XmlElement& xmlState, const File& songDirectory)
{
  // aquire mutex-locks for the sample-pool and the array of tracks:
  lockArrangement();
  samplePool->lockAudioFileBufferArray();
  tracks.getLock().enter();

  //songTitle             = xmlState.getStringAttribute("songTitle", String("My Song") );
  //lengthInBeats         = xmlState.getDoubleAttribute("songLengthInBeats",   180.0);
  setMaxTimeInBeats(      xmlState.getDoubleAttribute("songLengthInBeats",   180.0) );
  masterVolumeLevel     = xmlState.getDoubleAttribute("masterVolumeLevel",     0.0);
  globalTempoInBpm      = xmlState.getDoubleAttribute("tempo",               120.0);
  systemSampleRate      = xmlState.getDoubleAttribute("sampleRate",        44100.0);
  masterAmplitudeFactor = (float) dB2amp(masterVolumeLevel);

  // clear the array of tracks (this is an OwnedArray, so it will also delete the Track-objects, 
  // and Track's destructor will also delete the contained Clip objects):
  tracks.clear(true);

  // we must clear the clipboard because the clips that may be stored there have references to 
  // the audio-buffers in the samplePool (which will be cleared in turn and thus invalidate these 
  // references):
  clearClipBoard();

  // clear also the samplePool:
  samplePool->clear();

  // add new tracks according to the arrangement-data stored in the xmlState and restore 
  // the data of each track:
  Track*      track;
  AudioClip*  audioClip;
  int t = 0;
  forEachXmlChildElementWithTagName(xmlState, trackState, "TRACK")
  {
    // the child object 'trackState' is now guaranteed to be a <TRACK> element..
    track = new Track(t);
    track->setStateFromXml(*trackState);
    track->registerObserver(this);
    tracks.add(track);
    forEachXmlChildElementWithTagName(*trackState, clipState, "CLIP")
    {
      // the child object 'clipState' is now guaranteed to be a <CLIP> element..
      if( clipState->getStringAttribute("type") == String("audio") )
      {
        // retrieve the path of the underlying file (which is sipposed to be a relative path with
        // respect to 'songDirectory'):
        String newFileRelativePath = clipState->getStringAttribute("audioFileRelativePath");
        String newFileFullPath 
          = songDirectory.getFullPathName() + File::separatorString + newFileRelativePath;

        // create a file object from this path string - the function will return File::nonexistent 
        // when the file does not exist or is not a valid audiofile:
        File audioFile = getAudioFileFromPath(newFileFullPath);
        if( audioFile == File::nonexistent )
        {
          // open a warning box and leave the current iteration of the inner loop over the clips:
          showFileNotFoundOrInvalidAudioFileBox(newFileFullPath);
          continue;
          // \todo: this may be refined later to give the user the chance to re-locate the file
        }

        // check whether or not the samplePool has already buffered this file - if not, create a 
        // new AudioFileDisplayBuffer and add it to the pool:
        AudioFileBuffer* currentBuffer = NULL;
        int indexInPool = samplePool->findFileInPool(audioFile);
        if( indexInPool != -1 )
        {
          // file already buffered - retrieve and use it:
          currentBuffer = samplePool->getAudioFileBuffer(indexInPool);
        }
        else
        {
          // file not yet buffered - create a temporaray buffer, pass it over to the pool and 
          // delete the temporary buffer (the pool will create it's own copy). after this has been
          // done, retrieve and use it:
          AudioFileBuffer* tmpBuffer = new AudioFileBuffer( File(newFileFullPath) );
          indexInPool = samplePool->addIfNotAlreadyThere(tmpBuffer);   
          delete tmpBuffer;
          currentBuffer = samplePool->getAudioFileBuffer(indexInPool);
        }

        // create a new AudioClip object based on this buffer, set up its state-parameters 
        // according to the xml-element and add it to the Arrangement:
        audioClip = new AudioClip(currentBuffer);
        audioClip->setStateFromXml(*clipState);
        audioClip->setHostingTrackIndex(t);
        bool clipAdded = addClip(audioClip, t);
        if( clipAdded == false )
        {
          delete audioClip;
          jassertfalse;   // something went wrong in adding a clip
        }
        else
        {
          // if there is some clipSelectionListener set up, register it with the added clip:
          if( clipSelectionListener != NULL )
            audioClip->addSelectionListener(clipSelectionListener);
        }
      }
    }    
    t++; // increment track index
  }
  updateSoloedTrack();

  // release the aquired mutex-locks for the arrays:
  samplePool->unlockAudioFileBufferArray();
  tracks.getLock().exit();
  unlockArrangement();
}

//-------------------------------------------------------------------------------------------------
// others:

void Arrangement::lockArrangement()
{
  lock.enter();
}

void Arrangement::unlockArrangement()
{
  lock.exit();
}

//-------------------------------------------------------------------------------------------------
// internal functions:

void Arrangement::clearClipBoard()
{
  clipsInClipboard.getLock().enter();
  for(int c=0; c<clipsInClipboard.size(); c++)
    delete clipsInClipboard[c];
  clipsInClipboard.clear();
  clipsInClipboard.getLock().exit();
}

void Arrangement::updateSoloedTrack()
{
  lockArrangement();  
  tracks.getLock().enter();
  soloedTrack = -1;
  for(int t = 0; t < tracks.size(); t++)
  {
    if( tracks[t]->isSolo() )
      soloedTrack = t;
  }
  tracks.getLock().exit();
  unlockArrangement();
}
