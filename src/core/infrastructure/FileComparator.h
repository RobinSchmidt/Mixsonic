#ifndef FileComparator_h
#define FileComparator_h

#include "../../libs/JuceIncludes.h"

/**

This class compares two juce::File objects (for equality, less-than, greater-than condition) based
on a String comparison of the names of the files. This is required to sort a bunch of files in
alphabetical order, for example.

*/

class FileComparator
{
public:
 static int compareElements(const juce::File& first,
                            const juce::File& second) throw()
 {
  if( first.getFileName() < second.getFileName() )
   return -1;
  else if( first.getFileName() > second.getFileName() )
   return 1;
  else
   return 0;
 }
};

#endif 
