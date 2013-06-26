#include "Clip.h"
#include "ClipDeletionWatcher.h"
#include "ClipInstantiationManager.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction:

Clip::Clip() 
{
  init();
}

Clip::Clip(const Clip& other)
{
  name                        = other.name;
  position                    = other.position;
  totalDuration               = other.totalDuration;
  headingInactiveDuration     = other.headingInactiveDuration;
  trailingInactiveDuration    = other.trailingInactiveDuration;
  mute                        = other.mute;
  hostingTrackIndex           = other.hostingTrackIndex;
  instantiationManager        = other.instantiationManager;
}

Clip::~Clip()
{
  deletionWatchers.getLock().enter();

  // no for-loop here - the watchers may remove themselves in their callbacks which shrinks the 
  // array while we are iterating:
  while(deletionWatchers.size() > 0) 
    deletionWatchers[0]->clipIsToBeDeleted(this);

  deletionWatchers.getLock().exit();
}

//-------------------------------------------------------------------------------------------------
// callbacks:

void Clip::prepareToPlay(double sampleRate)
{

}

void Clip::addSignalToAudioBlock(const AudioSourceChannelInfo &bufferToFill, 
                                 const int sliceStartInSamples, 
                                 const double ampL, const double ampR)
{
  bufferToFill.clearActiveBufferRegion();
}

//-------------------------------------------------------------------------------------------------
// setup:

void Clip::init()
{
  name                     = String("Clip");
  position                 = 0.0;
  totalDuration            = 0.0;
  headingInactiveDuration  = 0.0;
  trailingInactiveDuration = 0.0;
  mute                     = false;
  hostingTrackIndex        = -1;
  instantiationManager     = NULL;
}

void Clip::setName(const String &newName)
{
  name = newName;
  deletionWatchers.getLock().enter();
  for(int i=0; i<deletionWatchers.size(); i++)
    deletionWatchers[i]->clipNameChanged(this);
  deletionWatchers.getLock().exit();
}

void Clip::setClipPosition(double newPosition, int timeUnit)
{
  position = convertTimeUnit(newPosition, timeUnit, false, BEATS, false);
}

void Clip::setActiveRegionStart(double newStart, int timeUnit, bool inLocalTime)
{
  headingInactiveDuration = convertTimeUnit(newStart, timeUnit, inLocalTime, BEATS, true);
}

void Clip::setActiveRegionEnd(double newEnd, int timeUnit, bool inLocalTime)
{
  double newEndLocal = convertTimeUnit(newEnd, timeUnit, inLocalTime, BEATS, true);
  trailingInactiveDuration = totalDuration - newEndLocal;
}

void Clip::setHostingTrackIndex(int newHostingTrackIndex)
{
  hostingTrackIndex = newHostingTrackIndex;
}

void Clip::addClipDeletionWatcher(ClipDeletionWatcher* watcherToAdd)
{
  deletionWatchers.getLock().enter();
  deletionWatchers.addIfNotAlreadyThere(watcherToAdd);
  deletionWatchers.getLock().exit();
}

void Clip::removeClipDeletionWatcher(ClipDeletionWatcher* watcherToRemove)
{
  deletionWatchers.getLock().enter();
  deletionWatchers.removeValue(watcherToRemove);
  deletionWatchers.getLock().exit();
}

void Clip::removeAllClipDeletionWatchers()
{
  deletionWatchers.getLock().enter();
  deletionWatchers.clear();
  deletionWatchers.getLock().exit();
}

//-------------------------------------------------------------------------------------------------
// inquiry:

String Clip::getName() const
{
  return name;
}

double Clip::getClipPosition(int timeUnit) const
{
  return convertTimeUnit(position, BEATS, false, timeUnit, false);
}

double Clip::getTotalDuration(int timeUnit) const
{
  return convertTimeUnit(totalDuration, BEATS, true, timeUnit, true);
}

double Clip::getActiveRegionDuration(int timeUnit) const
{
  double duration = totalDuration - headingInactiveDuration - trailingInactiveDuration;
  return convertTimeUnit(duration, BEATS, true, timeUnit, true);
}

double Clip::getActiveRegionStart(int timeUnit, bool inLocalTime) const
{
  return convertTimeUnit(headingInactiveDuration, BEATS, true, timeUnit, inLocalTime);
}

double Clip::getActiveRegionEnd(int timeUnit, bool inLocalTime) const
{
  double endLocal = getTotalDuration(BEATS) - trailingInactiveDuration;
  return convertTimeUnit(endLocal, BEATS, true, timeUnit, inLocalTime);
}

bool Clip::isTimeInstantInsideClip(double timeInstant, int timeUnit, bool inLocalTime) const
{
  double timeInstantLocal = convertTimeUnit(timeInstant, timeUnit, inLocalTime, BEATS, true);
  if( timeInstantLocal >= 0.0 && timeInstantLocal <= getTotalDuration(BEATS) )
    return true;
  else
    return false;
}

bool Clip::isTimeInstantInsideActiveRegion(double timeInstant, int timeUnit, bool inLocalTime) const
{
  double timeInstantLocal = convertTimeUnit(timeInstant, timeUnit, inLocalTime, BEATS, true);
  double regionStartLocal = headingInactiveDuration;
  double regionEndLocal   = getTotalDuration(BEATS) - trailingInactiveDuration;
  if( timeInstantLocal >= regionStartLocal && timeInstantLocal <= regionEndLocal )
    return true;
  else
    return false;
}

bool Clip::hasDataBetweenTimeInstants(double timeInstant1, double timeInstant2, int timeUnit, 
                                      bool inLocalTime) const
{
  double timeInstantLocal1 = convertTimeUnit(timeInstant1, timeUnit, inLocalTime, BEATS, true);
  double timeInstantLocal2 = convertTimeUnit(timeInstant2, timeUnit, inLocalTime, BEATS, true);

  if( timeInstantLocal1 == timeInstantLocal2 )
    return false; // no data in a timeslice of zero length
  else if( timeInstantLocal1 > timeInstantLocal2 )
    swap(timeInstantLocal1, timeInstantLocal2); // sort ascending

  if( timeInstantLocal1 > getTotalDuration(BEATS) )
    return false;
  if( timeInstantLocal2 < 0.0 )
    return false;
  else 
    return true;
}

bool Clip::hasActiveDataBetweenTimeInstants(double timeInstant1, double timeInstant2, int timeUnit, 
                                            bool inLocalTime) const
{
  if( timeInstant1 == timeInstant2 )
    return false; // no data in a timeslice of zero length
  else if( timeInstant1 > timeInstant2 )
    swap(timeInstant1, timeInstant2); // sort ascending

  if( timeInstant1 > getActiveRegionEnd(timeUnit, inLocalTime) )
    return false;
  else if( timeInstant2 < getActiveRegionStart(timeUnit, inLocalTime) )
    return false;
  else
    return true;
}

int Clip::getHostingTrackIndex() const
{
  return hostingTrackIndex;
}

//-------------------------------------------------------------------------------------------------
// callback based instantiation management:

void Clip::setInstantiationManager(ClipInstantiationManager *newInstantiationManager)
{
  instantiationManager = newInstantiationManager;
}

void Clip::sendDeletionRequest()
{
  if( instantiationManager != NULL )
    instantiationManager->handleDeletionRequest(this);
}

void Clip::sendSplitRequest(double timeInstantAtWhichToSplit, int timeUnit, bool inLocalTime)
{
  if( instantiationManager != NULL )
  {
    instantiationManager->handleSplitRequest(this, timeInstantAtWhichToSplit, timeUnit, 
      inLocalTime);
  }
}

//-------------------------------------------------------------------------------------------------
// state saving and recall:

XmlElement* Clip::getStateAsXml() const
{
  XmlElement* xmlState = new XmlElement(String("CLIP")); 

  xmlState->setAttribute("name",                     name);
  xmlState->setAttribute("position",                 position);
  xmlState->setAttribute("headingInactiveDuration",  headingInactiveDuration);
  xmlState->setAttribute("trailingInactiveDuration", trailingInactiveDuration);
  xmlState->setAttribute("isSelected",               selected);
  xmlState->setAttribute("mute",                     mute);

  return xmlState;
}

void Clip::setStateFromXml(const juce::XmlElement &xmlState)
{
  name                     = xmlState.getStringAttribute("name",                       "Clip");
  position                 = xmlState.getDoubleAttribute("position",                      0.0);
  headingInactiveDuration  = xmlState.getDoubleAttribute("headingInactiveDuration",       0.0);
  trailingInactiveDuration = xmlState.getDoubleAttribute("trailingInactiveDuration",      0.0);
  selected                 = xmlState.getBoolAttribute(  "isSelected",                  false);
  mute                     = xmlState.getBoolAttribute(  "mute",                        false);
}

//-------------------------------------------------------------------------------------------------
// internal functions:

double Clip::convertTimeUnit(const double in, const int inUnit, const bool inTimeIsLocal, 
                             const int outUnit, const bool outTimeIsLocal) const
{
  double tmp = in;

  // convert global to local or vice versa if necesarry:
  if( inTimeIsLocal && !outTimeIsLocal )
    tmp += getClipPosition(inUnit);
  else if( !inTimeIsLocal && outTimeIsLocal )
    tmp -= getClipPosition(inUnit);

  // convert the unit and return the value:
  return TimeUnitConverter::convertTimeUnit(tmp, inUnit, outUnit, 
                                            getSystemSampleRate(), getGlobalTempoInBpm());
}