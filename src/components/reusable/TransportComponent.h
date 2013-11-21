#ifndef TransportComponent_h
#define TransportComponent_h

#include "../../control/TransportController.h"
#include "../reusable/UserInterfaceSection.h"
#include "../widgets/RButton.h"

class TransportComponent : public Component, public ButtonListener, public UserInterfaceSection
{
public:

  TransportComponent(SectionSkin *skinToUse, TransportController *controllerToUse);

  virtual ~TransportComponent();


  /** Computes the width required by this component given a desired height. */
  virtual int getRequiredWidth(int height);

  // overrides:
  virtual void buttonClicked(Button *buttonThatWasClicked);
  virtual void paint(Graphics& g);
  virtual void resized();


  static const int margin         = 4; // margin between buttons and border
  static const int buttonDistance = 2; // distance between buttons

protected:

  RButton *playButton, *rewindButton, *largeBackwardStepButton, *largeForwardStepButton;
    /* *smallBackwardStepButton; *smallForwardStepButton, *forwardToEndButton; */
  // later: include: *loopButton
  // maybe we should get rid of the two different size steps (jump only to markers) and also
  // of the forwardToEndButton - too much clutter for little value




  TransportController *transportController;

};

#endif