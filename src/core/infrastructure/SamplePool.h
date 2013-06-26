#ifndef SamplePool_h
#define SamplePool_h

#include "../audio/AudioFileBufferUser.h"
class SamplePoolClient;

/**

This class is to be used for maintaining a pool of audio samples in form of an array of 
AudioFileDisplayBuffer objects. It provides means to add/move and delete samples from the pool and 
a notification system to inform other objects that may be affected by such events.

*/

class SamplePool
{  

public:

  //-----------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  SamplePool();  

  /** Destructor. */
  virtual ~SamplePool();                             

  //-----------------------------------------------------------------------------------------------
  // setup:

  /** Sets up the directory into which the samples are copied. */
  virtual void setSampleDirectory(const File& newSampleDirectory);

  /** Adds a sample to the pool and optionally copies the underlying file into the project 
  directory - the sample must be passed as object of class AudioFileBuffer. The return 
  value is the index of the buffer inside the pool. WARNING: if you want to use that index for 
  something, make sure to wrap all these actions (including the call to this add... function) in 
  between lockAudioFileBufferArray()/lockAudioFileBufferArray() to make sure that the index is not 
  invalidated by another thread while you are using it. */
  virtual int addIfNotAlreadyThere(AudioFileBuffer* bufferToAdd);

  /** Checks, if a buffer with some particular underlying file is already in the pool - if so, it 
  returns its index, if not, it returns -1. WARNING: if you want to use that index for 
  something, make sure to wrap all these actions (including the call to this find... function) in 
  between lockAudioFileBufferArray()/lockAudioFileBufferArray() to make sure that the index is not 
  invalidated by another thread while you are using it. */
  virtual int findFileInPool(const File& fileToCheckFor);

  /** Renames a sample file and notifies our clients via callback to 
  SamplePoolClient::samplePathHasChanged() for each samplefile that is currently loaded into the 
  RAM. */
  virtual bool renameSampleFile(const File& fileToRename, const File& targetFile);

  /** Renames a whole directory and takes care that all for audiofiles in it (or in one of its 
  subdirectories) which are currently loaded into the RAM, the notification
  SamplePoolClient::samplePathHasChanged() will be called back for our clients. */
  virtual bool renameSampleDirectory(const File& directoryToRename, const File& targetDirectory);

  /** Moves a sample file to a new location and notifies our clients via callback to 
  SamplePoolClient::samplePathHasChanged() for each samplefile that is currently loaded into the 
  RAM.  */
  virtual bool moveSampleFileInto(const File& fileToMove, const File& targetDirectory);

  /** Moves a whole directory with all it's subdirectories to a new location and notifies our 
  clients via callback to SamplePoolClient::samplePathHasChanged() for each sample that is 
  currently loaded into the RAM. */
  virtual bool moveDirectoryInto(const File& directoryToMove, const File& targetDirectory);

  /** Removes a sample from the sample pool and optionally deletes the underlying file from the 
  harddisc (physically, so use it with care!). Before doing so, it will call back our clients 
  through their sampleIsToBeDeleted method in which they should clear their references to the 
  sample. */
  virtual void removeSampleFromPool(const File& fileToRemove, bool deletePhysically);

  /** Recursively removes a directory from the pool and optionally deletes it physically from the 
  harddisc. */
  virtual void removeDirectoryFromPool(const File& directoryToRemove, bool deletePhysically);

  /** Clears the sample pool (only the RAM - without deleting any files from the HD). */
  virtual void clear();

  /** Adds a client to this SamplePool. The client will be called back when something about this 
  SamplePool object has changed. */
  virtual void addClient(SamplePoolClient* clientToAdd);

  /** Removes a client from this SamplePool object. */
  virtual void removeClient(SamplePoolClient* clientToRemove);

  //-----------------------------------------------------------------------------------------------
  // inquiry:

  /** Searches our array of AudioFileBuffers 'audioFileBuffers' for the element 'bufferToLookFor' 
  and returns its index when the element is found, -1 otherwise. The comparison relies on the '==' 
  operator defined in class AudioFileDisplayBuffer. WARNING: that index is only guaranteed to be 
  valid upon return when you wrap it into lockAudioFileBufferArray()/unlockAudioFileBufferArray() 
  as other threads may modify the SamplePool.*/
  virtual int getAudioFileBufferIndexInPool(const AudioFileBuffer* bufferToLookFor) const;

  /** Returns a pointer to the buffer with given index or a NULL pointer when the index does not 
  exist. WARNING: when retrieving that pointer and doing something with it, you should call 
  lockAudioFileBufferArray() before the retrieval and unlockAudioFileBufferArray() after you 
  finished your work to make it thread-safe. */
  virtual AudioFileBuffer* getAudioFileBuffer(int index);

  /** Returns the sampleDirectory. */
  const File& getSampleDirectory() const;

  //-----------------------------------------------------------------------------------------------
  // others:

  /** Acquires the lock for the internal array of AudioFileDisplayBuffer objects. */
  void lockAudioFileBufferArray();

  /** Releases the lock for the internal array of AudioFileDisplayBuffer objects. */
  void unlockAudioFileBufferArray();

  //===============================================================================================
  juce_UseDebuggingNewOperator;

protected:

  /** Moves a sample file to a new location (this may also be another filename in the same 
  directory). */
  bool moveSampleFile(const File& oldFile, const File& newFile);

  /** Moves a directory to a new location - this may also be another directory name in the same 
  parent directory, that is: it may be used to rename existing directories. */
  bool moveDirectory(const File& oldDirectory, const File& newDirectory);

  /** Notifies our clients that some sample path has changed. */
  void sendSamplePathChangeNotification(int index, const File& oldPath, const File& newPath);

  /** Notifies our clients that some sample will be deleted. */
  void sendSampleToBeDeletedNotification(int index);

  /** This is the pool of samples which are used inside the arrangement. */
  OwnedArray<AudioFileBuffer, CriticalSection> audioFileBuffers;

  // the root directory for the pool of sample files:
  File sampleDirectory;

  // our clients that get notified when something relevant changed:
  Array<SamplePoolClient*, CriticalSection> clients;

};

#endif  