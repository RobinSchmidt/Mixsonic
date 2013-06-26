#ifndef DirectoryTreeComponent_h
#define DirectoryTreeComponent_h

#include "../../libs/JuceIncludes.h"

/**

/ /old comment
This class is merely a copy-and-paste of juce::FileTreeComponent with one minor difference: it 
shows the +/- buttons in front of the directories only when there are subdirectories (whereas) the 
juce::FileTreeComponent shows them whenever it sees a directory. It is used by 
MixsonicSampleBrowser.

\todo: do the copy/paste again with the code from the new juce version

or: just derive from juce::FileTreeComponent and override the paint method

*/

class DirectoryTreeComponent : public FileTreeComponent
{
public:

    DirectoryTreeComponent(DirectoryContentsList& listToShow);

    //==============================================================================
    juce_UseDebuggingNewOperator


      
    // \todo: override the paint method such that the opener "+" only appears, if the directory
    // has subdirectories

private:

    DirectoryTreeComponent (const DirectoryTreeComponent&);
    const DirectoryTreeComponent& operator= (const DirectoryTreeComponent&);
};


#endif   // DirectoryTreeComponent_h
