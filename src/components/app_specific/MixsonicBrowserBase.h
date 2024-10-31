#ifndef MixsonicBrowserBase_h
#define MixsonicBrowserBase_h

#include "MixsonicSubBrowser.h"
#include "../widgets/MixsonicButton.h"
#include "../widgets/MLabel.h"
#include "../../core/audio/ImmediatePlaybackAudioSource.h"
#include "../../core/infrastructure/TimeUnitConverter.h"
#include "../../core/infrastructure/FileFilters.h"

/**

This class serves as baseclass for the file-browser (top-left) and project-browser (top-right) in
the Mixsonic application. It contains their common code.

*/

class MixsonicBrowserBase : public Component, public ButtonListener, public ResamplingAudioSource,
  public UserInterfaceSection
{  

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  MixsonicBrowserBase(SectionSkin *skinToUse, const File &directoryToUse);  
    // \todo the parameter directoryToUse can be dropped

  /** Destructor. */
  virtual ~MixsonicBrowserBase();           

  //-----------------------------------------------------------------------------------------------
  // setup: 

  /** Sets the Label in which the descriptions for the clips will appear. */
  virtual void setDescriptionField(Label* newDescriptionField);

  //-----------------------------------------------------------------------------------------------
  // callbacks:

  /** Implements the purely virtual buttonClicked()-method of the ButtonListener base-class. */
  virtual void buttonClicked(Button *buttonThatWasClicked);

  /** Overrides the setBounds()-method of the Component base-class in order to arrange the 
  widgets according to the size. */
  virtual void resized();

  //-----------------------------------------------------------------------------------------------
  // others:

  /** Starts the preview-playback of an audiofile, but only if the current configuration of the 
  play- and pause-button calls for this (i.e. play is down, pause is up). */
  virtual void startPlaybackIfDesired(const File &fileToPlay);

  /** Starts the preview-playback of an audiofile. */
  virtual void startPlayback(const File &fileToPlay);

  /** Pauses the playback for later continuation (maybe also used to releasr the pause state by 
  passing false. */
  virtual void pausePlayback(bool shouldBePaused);

  /** Stops the preview-playback of an audiofile. */
  virtual void stopPlayback();

  /** Restarts the preview-playback from the beginning of the audiofile. */
  virtual void restartPlayback();


protected:

  // the headline for the component:
  MHeadlineLabel *headline;

  // the buttons themselves:
  MixsonicButton *pauseButton, *playButton, *rewindButton; 
    // \todo get rid of pauseButton, rewindButton, rename playButton to preListenButton

  // thread to be used for (...tbc):
  TimeSliceThread browserUpdateThread;

  // stuff for the pre-listen functionality:
  ImmediatePlaybackAudioSource inputSource;
  AudioFileBuffer              audioBuffer;


  JUCE_LEAK_DETECTOR(MixsonicBrowserBase);
};

#endif  