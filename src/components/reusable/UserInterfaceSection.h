#ifndef UserInterfaceSection_h
#define UserInterfaceSection_h

#include "../../core/infrastructure/DescribedItem.h"
#include "../../control/Skin.h"

// \todo rename file to UserInterface (or something similar)

/**

This class serves as base class for various sections of a user interface and provides functionality
for defining a skin.

*/

class UserInterfaceSection : public DescribedMouseListener
{

public:

  /** Constructor. */
  UserInterfaceSection(SectionSkin *skinToUse = nullptr, 
                       const String& newDescription = String("User Interface Section"));

protected:

  SectionSkin *skin;

  JUCE_LEAK_DETECTOR(UserInterfaceSection);
};

//=================================================================================================

/**

This class serves as base class for a user interface for a full application (possibly containing
many sections).

*/

class ApplicationUserInterface
{
public:
  ApplicationUserInterface(ApplicationSkin *skinToUse = nullptr);
protected:
  ApplicationSkin *skin;
  JUCE_LEAK_DETECTOR(ApplicationUserInterface);
};


#endif  
