#ifndef MixsonicTextEntryField_h
#define MixsonicTextEntryField_h

#include "RWidget.h"

/**

This class is a juce::TextEditor extended by the functionality of RWidget. 

*/

class MixsonicTextEntryField : public TextEditor, public RWidget
{

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  MixsonicTextEntryField(const String& initialText = String::empty);

  /** Destructor. */
  virtual ~MixsonicTextEntryField();

  //-----------------------------------------------------------------------------------------------
  // others:

  /** Overrides the mouseEnter callback in order to show the description in the dedicated field 
  when the mouse enters the widget. */
  virtual void mouseEnter(const MouseEvent &e);

  /** Overrides the mouseExit callback in order to make the description disappear when the mouse 
  leaves the widget. */
  virtual void mouseExit(const MouseEvent &e);

  //===============================================================================================
  juce_UseDebuggingNewOperator;

};

#endif  
