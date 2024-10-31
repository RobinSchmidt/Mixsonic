#include "TransportComponent.h"

TransportComponent::TransportComponent(SectionSkin *skinToUse, 
  TransportController *controllerToUse)
: UserInterfaceSection(skinToUse)
{
  transportController = controllerToUse;

  playButton = new MButton(&skin->widgetSkin, MButton::PLAY);
  addAndMakeVisible(playButton);
  playButton->setDescription("Start or stop playback");
  playButton->addShortcut(KeyPress::spaceKey);
  playButton->addShortcut(KeyPress::playKey);
  playButton->setClickingTogglesState(true);
  playButton->addListener(this);

  rewindButton = new MButton(&skin->widgetSkin); // \todo 2nd parameter MButton::REWIND
  addAndMakeVisible(rewindButton);
  rewindButton->setDescription("Rewind to start");
  rewindButton->addShortcut(KeyPress::homeKey);
  rewindButton->setClickingTogglesState(false);
  rewindButton->addListener(this);

  largeBackwardStepButton = new MButton(&skin->widgetSkin, MButton::SKIP_BACK);
  addAndMakeVisible(largeBackwardStepButton);
  largeBackwardStepButton->setDescription("Skip back to previous marker");
  largeBackwardStepButton->addShortcut(KeyPress::leftKey);
  largeBackwardStepButton->addShortcut(KeyPress::rewindKey);
  largeBackwardStepButton->setClickingTogglesState(false);
  largeBackwardStepButton->addListener(this);

  largeForwardStepButton = new MButton(&skin->widgetSkin, MButton::SKIP_FORWARD);
  addAndMakeVisible(largeForwardStepButton);
  largeForwardStepButton->setDescription("Skip forward to next marker");
  largeForwardStepButton->addShortcut(KeyPress::rightKey);
  largeForwardStepButton->addShortcut(KeyPress::fastForwardKey);
  largeForwardStepButton->setClickingTogglesState(false);
  largeForwardStepButton->addListener(this);

  /*
  addAndMakeVisible( smallBackwardStepButton = new MButton(MButton::SKIP_BACK) );
  smallBackwardStepButton->setDescription("Skip back one measure"); // or 10 seconds?
  smallBackwardStepButton->addShortcut(KeyPress::leftKey);
  smallBackwardStepButton->addShortcut(KeyPress::rewindKey);
  smallBackwardStepButton->setClickingTogglesState(false);
  smallBackwardStepButton->addListener(this);

  addAndMakeVisible( smallForwardStepButton = new MButton(MButton::SKIP_FORWARD) );
  smallForwardStepButton->setDescription("Skip forward one measure"); // or 10 seconds?
  smallForwardStepButton->addShortcut(KeyPress::rightKey);
  smallForwardStepButton->addShortcut(KeyPress::fastForwardKey);
  smallForwardStepButton->setClickingTogglesState(false);
  smallForwardStepButton->addListener(this);

  addAndMakeVisible( forwardToEndButton = new MButton() );
  forwardToEndButton->setDescription("Forward to end");
  forwardToEndButton->addShortcut(KeyPress::endKey);
  forwardToEndButton->setClickingTogglesState(false);
  forwardToEndButton->addListener(this);
  */
}

TransportComponent::~TransportComponent()
{
  deleteAllChildren();
}

int TransportComponent::getRequiredWidth(int height)
{
  int w = height - 2*margin; // button width
  return 2*margin + 3*w + 3*w/2 + 3*buttonDistance;
}

void TransportComponent::buttonClicked(Button *button)
{
  if( button == playButton )
    transportController->togglePlayback(playButton->getToggleState());
  else if( button == rewindButton )
    transportController->rewindToStart();
  else if( button == largeBackwardStepButton )
    transportController->largeStepBackward();
  //else if( button == smallBackwardStepButton )
  //  transportController->smallStepBackward();
  else if( button == largeForwardStepButton )
    transportController->largeStepForward();
  //else if( button == smallForwardStepButton )
  //  transportController->smallStepForward();
}

void TransportComponent::paint(Graphics& g)
{
  //g.setColour(skin->outlineColor);
  g.setColour(Colours::red); // preliminary

  g.drawRect(0.f, 0.f, (float)getWidth(), (float)getHeight(), 2.f);
}

void TransportComponent::resized()
{
  //int m = 4;                    // margin to component bounds
  //int d = 2;                      // distance between buttons
  int h = getHeight() - 2*margin; // button height
  int w = h;                      // button width
  int x = margin;
  int y = margin;

  rewindButton->setBounds(x, y, w, h);
  x = rewindButton->getRight() + buttonDistance;
  largeBackwardStepButton->setBounds(x, y, w, h);
  x = largeBackwardStepButton->getRight() + buttonDistance;
  //smallBackwardStepButton->setBounds(x, y, w, h);
  //x = smallBackwardStepButton->getRight() + buttonDistance;
  playButton->setBounds(x, y, 3*w/2, h);
  x = playButton->getRight() + buttonDistance;
  //smallForwardStepButton->setBounds(x, y, w, h);
  //x = smallForwardStepButton->getRight() + buttonDistance;
  largeForwardStepButton->setBounds(x, y, w, h);
  //x = largeForwardStepButton->getRight() + buttonDistance;
  //forwardToEndButton->setBounds(x, y, w, h);
}