#ifndef MixsonicSliderListener_h
#define MixsonicSliderListener_h

class MixsonicSlider;

class MixsonicSliderListener
{

public:

  virtual void mSliderValueChanged(MixsonicSlider* mSlider) = 0;

  JUCE_LEAK_DETECTOR(MixsonicSliderListener);
};

#endif
