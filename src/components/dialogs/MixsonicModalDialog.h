#ifndef MixsonicModalDialog_h
#define MixsonicModalDialog_h

#include "../reusable/UserInterfaceSection.h"

/**

This class serves as baseclass for various modal dialogs in the Mixsonic application.

*/

class MixsonicModalDialog : public Component, virtual public UserInterfaceSection
{

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  MixsonicModalDialog(SectionSkin *skinToUse);

  /** Destructor. */
  virtual ~MixsonicModalDialog();

  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Sets up the headline for the dialog. */
  virtual void setHeadline(const String& newHeadline);

  //-----------------------------------------------------------------------------------------------
  // callbacks:

  /** Overrides paint to fill the background. */
  virtual void paint(Graphics &g);

  /** Overrides resized()-method in order to position the headline. */
  virtual void resized();

  //===============================================================================================
  juce_UseDebuggingNewOperator;
  
protected:

  // the headline for the dialog:
  Label* headlineLabel;

};

#endif
