#ifndef SamplePoolClient_h
#define SamplePoolClient_h

#include "SamplePool.h"

/**

This class is used as baseclass for all objects that need access to a SamplePool object - be it for 
playback or for manipulating the samples in the pool.

*/

class SamplePoolClient
{  

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  SamplePoolClient(SamplePool *samplePoolToUse);  

  /** Destructor. */
  virtual ~SamplePoolClient();           

  //-----------------------------------------------------------------------------------------------
  // callbacks:

  /** Implement this method to respond to the change of a path of a sample in the pool (due to 
  moving or renaming the file). */
  virtual void samplePathHasChanged(int index, const File& oldFilePath, 
    const File& newFilePath) = 0;

  /** Implement this method to clear your references to some sample file in the pool because the 
  file is going to be removed from the pool and possible even deleted from the HD. */
  virtual void sampleIsToBeDeleted(int index) = 0;

  //===============================================================================================
  juce_UseDebuggingNewOperator;

protected:

  // the SamplePool object to be used/manipulated:
  SamplePool *samplePool;

};

#endif  