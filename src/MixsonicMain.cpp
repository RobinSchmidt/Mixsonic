#include "components/app_specific/MixsonicContentComponent.h"
#include "components/app_specific/MixsonicLookAndFeel.h"

/** 

This is the top-level window that we'll pop up. Inside it, we'll create and show a 
MixsonicContentComponent component.

*/

class MixsonicWindow : public DocumentWindow, public ChangeListener
{

public:

  MixsonicWindow() : DocumentWindow("Mixsonic", Colours::lightgrey, 
    DocumentWindow::allButtons, true)
  {
    contentComponent = new MixsonicContentComponent(String("Mixsonic"));
    setName(String("Mixsonic - ") + contentComponent->getProjectName());
    contentComponent->addChangeListener(this);

    // enable command-invocation by keystrokes (the keystrokes are received by the main window and 
    // we need to make sure that they are passed through to the command-manager ...i guess):
    contentComponent->commandManager.getKeyMappings()->resetToDefaultMappings();
    addKeyListener(contentComponent->commandManager.getKeyMappings());

    setContentOwned(contentComponent, true); 

    setResizable(true, true);
    setResizeLimits(960, 640, 4000, 3000);
    setVisible(true);
    centreWithSize(1024, 768);
    //setUsingNativeTitleBar(true);
    setUsingNativeTitleBar(false);
  }

  ~MixsonicWindow()
  {
    // the content component will be deleted automatically, so no need to do
    // it here
  }

  //virtual void changeListenerCallback(void *objectThatHasChanged) // 1.46
  virtual void changeListenerCallback(ChangeBroadcaster *objectThatHasChanged) 
  {
    setName(String("Mixsonic - ") + contentComponent->getProjectName());
  }

  void closeButtonPressed()
  {
    // When the user presses the close button, we'll tell the app to quit. This 
    // window will be deleted by the app object as it closes down.
    if( !contentComponent->projectFileIsUpToDate )
    {
      if( showSongUnsavedWarningBox() == true )
        JUCEApplication::quit();   
      else
        return;
    }   
    else
      JUCEApplication::quit();   
  }

protected:

  MixsonicContentComponent* contentComponent;

};


//=============================================================================
/** 
This is the application object that is started up when Juce starts. It handles
the initialisation and shutdown of the whole application.
*/

class Mixsonic : public JUCEApplication
{

  /* Important! NEVER embed objects directly inside your JUCEApplication class! 
  Use ONLY pointers to objects, which you should create during the 
  initialise() method (NOT in the constructor!) and delete in the shutdown() 
  method (NOT in the destructor!)

  This is because the application object gets created before Juce has been 
  properly initialised, so any embedded objects would also get constructed 
  too soon.
  */

  MixsonicWindow* theWindow;

  MixsonicLookAndFeel lookAndFeel;

public:

  Mixsonic() : theWindow (0)
  {
    // NEVER do anything in here that could involve any Juce function being 
    // called - leave all your startup tasks until the initialise() method.
  }

  ~Mixsonic()
  {
    // Your shutdown() method should already have done all the things necessary to
    // clean up this app object, so you should never need to put anything in
    // the destructor.

    // Making any Juce calls in here could be very dangerous...
  }

  void initialise (const String& commandLine)
  {
    mixsonicGlobals = new MixsonicGlobals();

    LookAndFeel::setDefaultLookAndFeel(&lookAndFeel);

    // just create the main window...
    theWindow = new MixsonicWindow();

    /*  ..and now return, which will fall into to the main event
    dispatch loop, and this will run until something calls
    JUCEAppliction::quit().

    In this case, JUCEAppliction::quit() will be called by the
    hello world window being clicked.
    */
  }

  void shutdown()
  {
    // clean up:
    if (theWindow != 0)
      delete theWindow;
    delete mixsonicGlobals;
  }

  const String getApplicationName()
  {
    return "Mixsonic";
  }

  const String getApplicationVersion()
  {
    return "1.0";
  }

  bool moreThanOneInstanceAllowed()
  {
    return true;
  }

  void anotherInstanceStarted (const String& commandLine)
  {

  }
  
};


//==============================================================================
// This macro creates the application's main() function..
START_JUCE_APPLICATION (Mixsonic)
