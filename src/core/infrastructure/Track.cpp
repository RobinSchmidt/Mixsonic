#include "Track.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction:

Track::Track(int newIndex) 
{
  name  = String("Track");
  index = newIndex;
  mute  = false;
  solo  = false;
  level = 0.0;
  pan   = 0.0;
  amp   = 1.0;

  // we initially should have 2 empty plugin slots:
  //pluginChain.addEmptySlot();
  //pluginChain.addEmptySlot();
}

Track::~Track()
{
  // we need to delete the actual objects to which we hold the pointers:
  audioClips.getLock().enter();
  while( audioClips.size() > 0 )
  {
    delete audioClips[0]; // delete the object
    audioClips.remove(0); // remove the pointer from the array
  }
  // why no for-loop? has this to do with callbacks that get triggered from the destructor of
  // AudioClip?
  audioClips.getLock().exit();


  // \todo: do the same thing with our array of audioPlugins - perhaps, we should also make sure 
  // that no editor is open and no other objects maintain references to the to-be-deleted 
  // AudioPluginInstance


}

//-------------------------------------------------------------------------------------------------
// setup:

void Track::setVolumeLevel(double newLevel)
{
  level = newLevel;
  amp   = dB2amp(level);
}

void Track::setPan(double newPan)
{
  pan = newPan;
}

void Track::setSolo(bool shouldBeSolo) 
{ 
  solo = shouldBeSolo; 
  for(int i = 0; i < observers.size(); i++)
    observers[i]->trackSoloStateChanged(this, solo);
}

void Track::setIndex(int newIndex)
{
  jassert(newIndex >= 0);
  if( index >= 0 )
    index = newIndex;
}
 
void Track::registerObserver(TrackObserver *observer)
{
  observers.addIfNotAlreadyThere(observer);
}

void Track::deregisterObserver(TrackObserver *observer)
{
  observers.removeValue(observer);
}

//-------------------------------------------------------------------------------------------------
// inquiry:

bool Track::isClipSelected(int clipIndex)
{
  bool result = false;
  audioClips.getLock().enter();
  if( clipIndex >= 0 && clipIndex < audioClips.size() )
    result = audioClips[clipIndex]->isSelected();
  audioClips.getLock().exit();
  return result;
}

//-------------------------------------------------------------------------------------------------
// add and remove clips:

void Track::deSelectAllClips()
{
  audioClips.getLock().enter();
  for(int c=0; c<audioClips.size(); c++)
    audioClips[c]->setSelected(false, false);
  audioClips.getLock().exit();
}

void Track::addClip(AudioClip* clipToAdd)
{
  audioClips.getLock().enter();
  audioClips.add(clipToAdd);
  clipToAdd->prepareToPlay(TimeUnitConverter::getSystemSampleRate());
  clipToAdd->setHostingTrackIndex(index);
  audioClips.getLock().exit();
}

bool Track::removeClip(AudioClip *clipToRemove, bool deleteObject)
{
  audioClips.getLock().enter();
  int  index   = audioClips.indexOf(clipToRemove);
  bool success = removeClip(index, deleteObject);
  audioClips.getLock().exit();
  return success;
}

bool Track::removeClip(int index, bool deleteObject)
{
  bool success;
  audioClips.getLock().enter();
  if( index >= 0 && index < audioClips.size() )
  {
    //audioClips[index]->setHostingTrackIndex(-1); // no - we need to preserve that info
    if( deleteObject == true )
      delete audioClips[index];
    audioClips.remove(index);
    success = true;
  }
  else
  {
    success = false;
    jassertfalse; // should not happen that we try to remove a non-existing clip
  }
  audioClips.getLock().exit();
  return success;
}

void Track::removeAllClips(bool deleteObjects)
{
  audioClips.getLock().enter();
  while( audioClips.size() > 0 )
    removeClip(audioClips.size()-1, deleteObjects);
  audioClips.getLock().exit();
}


void Track::removeAllClipsUsingBuffer(AudioFileBuffer *buffer, bool deleteObjects)
{
  audioClips.getLock().enter();
  for(int c=0; c<audioClips.size(); c++)
  {
    if( audioClips[c]->isUsingBuffer(buffer) )
    {
      removeClip(c, deleteObjects);
      c--;
    }
  }
  audioClips.getLock().exit();
}

AudioClip* Track::getClipCopy(int index)
{
  AudioClip* returnedClip = NULL;

  audioClips.getLock().enter();
  if( index >= 0 && index < audioClips.size() )
  {
    // use the copy constructor to create the new clip:
    returnedClip = new AudioClip( *audioClips[index] );
  }
  audioClips.getLock().exit();

  return returnedClip;
}

void Track::deleteSelectedClips()
{
  audioClips.getLock().enter();
  for(int c=0; c<audioClips.size(); c++)
  {
    if( audioClips[c]->isSelected() )
    {
      delete audioClips[c];  // deletion of the actual object
      audioClips.remove(c);  // removal of the pointer from the array
      c--;                   // because the array has just shrunken
    }
  }
  audioClips.getLock().exit();
}
 
/*
void Track::addAudioPlugIn(AudioPluginInstance* pluginToAdd)
{
  const ScopedLock lock(audioPlugins.getLock());
  audioPlugins.add(pluginToAdd);
}

void Track::removeAudioPlugin(AudioPluginInstance* pluginToRemove, bool deleteObject)
{
  const ScopedLock lock(audioPlugins.getLock());
  audioPlugins.removeValue(pluginToRemove);
  if( deleteObject )
    delete pluginToRemove;
}
*/
/*
void Track::insertPlugin(int slotIndex, AudioPluginInstance* pluginToInsert)
{
  jassert( slotIndex >= 0 && slotIndex < pluginSlots.size() );
  int dummy = 0;

  // \todo if the last slot is not empty anymore, create a new slot
  // maybe we should have a function updateNumPluginSlots() which makes sure that there's always 
  // one (and only one) empty slot at the end of the array. this should probably also send out a 
  // callback message to our TrackObservers
}
*/

//-------------------------------------------------------------------------------------------------
// audio related callbacks:

void Track::prepareToPlay(double sampleRate)
{
  audioClips.getLock().enter();
  for(int c=0; c<audioClips.size(); c++)
    audioClips[c]->prepareToPlay(sampleRate);
  audioClips.getLock().exit();
}

void Track::addSignalToAudioBlock(const AudioSourceChannelInfo &bufferToFill,                                           
                                  const int64 sliceStartInSamples)
{
  if( mute )
    return;

  // Accumulate the signals from all clips on this track (clips may overlap - in this case we want
  // to sum the signals from the overlapping clips). The clips will themselves detect whether or 
  // not they have some signal inside the current time-slice and return immediately if they don't.
  // Maybe we could optimize this by maintaining an array of possibly active clips and iterate 
  // only over that (smaller) array. This array should always contain all clips which have their 
  // end-sample after or at the start-sample of this block and their start-sample before or at the 
  // end-sample of this block. It would have to be initialized in a prepareToPlay() function and 
  // updated here in each call. But perhaps this complication would do more harm than good.
  audioClips.getLock().enter();
  for(int i = 0; i < audioClips.size(); i++)
    audioClips[i]->addSignalToAudioBlock(bufferToFill, sliceStartInSamples, amp, pan);
  audioClips.getLock().exit();


  MidiBuffer dummyMidiBuffer;
  pluginChain.processBlock(bufferToFill, dummyMidiBuffer);
}

//-------------------------------------------------------------------------------------------------
// state saving and recall:

XmlElement* Track::getStateAsXml(const File& songDirectory) const
{
  XmlElement* xmlState = new XmlElement(String("TRACK"));

  xmlState->setAttribute("name",  name);
  xmlState->setAttribute("level", level);
  xmlState->setAttribute("pan",   pan);
  xmlState->setAttribute("mute",  mute);
  xmlState->setAttribute("solo",  solo);

  // create a child element for each clip and add it to the state-element:
  audioClips.getLock().enter();
  for(int c=0; c<audioClips.size(); c++)
  {
    XmlElement* clipState = audioClips[c]->getStateAsXml(songDirectory);
    xmlState->addChildElement(clipState);
  }
  audioClips.getLock().exit();

  xmlState->addChildElement(pluginChain.getStateAsXml());

  return xmlState;
}

void Track::setStateFromXml(const juce::XmlElement &xmlState)
{
  name  = xmlState.getStringAttribute("name",  "Track");
  level = xmlState.getDoubleAttribute("level",        0.0);
  amp   = dB2amp(level);
  pan   = xmlState.getDoubleAttribute("pan",          0.0);
  mute  = xmlState.getBoolAttribute(  "mute",       false);
  solo  = xmlState.getBoolAttribute(  "solo",       false);


  // \todo retrieve the state of the plugin-chain
}

