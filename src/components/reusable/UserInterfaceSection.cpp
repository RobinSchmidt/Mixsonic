#include "UserInterfaceSection.h"

UserInterfaceSection::UserInterfaceSection(SectionSkin *skinToUse, const String& newDescription)
: DescribedMouseListener(newDescription)
{
  skin = skinToUse;
  jassert(skin != nullptr);
}


ApplicationUserInterface::ApplicationUserInterface(ApplicationSkin *skinToUse)
{
  skin = skinToUse;
  jassert(skin != nullptr);
}

