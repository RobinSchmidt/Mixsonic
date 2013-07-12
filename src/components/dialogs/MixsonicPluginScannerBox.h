#ifndef MixsonicPluginScannerBox_h
#define MixsonicPluginScannerBox_h

#include "MixsonicModalDialog.h"
#include "../widgets/RLabel.h"

/**

This class implements a message box component for showing the progress of a scanning process of
the plugin directories. (it's actually not really a dialog)

*/

class MixsonicPluginScannerBox : public MixsonicModalDialog
{

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  MixsonicPluginScannerBox();

  /** Destructor. */
  virtual ~MixsonicPluginScannerBox();

  //-----------------------------------------------------------------------------------------------
  // callbacks:

  /** Overrides resized()-method of the Component base-class in order to arrange the widgets
  according to the size. */
  virtual void resized();

  //-----------------------------------------------------------------------------------------------
  // misc:

  virtual void scanPluginsAndShowProgress(PluginDirectoryScanner& scanner);

  //===============================================================================================
  juce_UseDebuggingNewOperator;
  
protected:

  // the widgets:
  RLabel *currentFileLabel;

};

#endif
