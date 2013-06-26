#include "SamplePool.h"
#include "SamplePoolClient.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction:

SamplePool::SamplePool() 
{
	sampleDirectory = getApplicationDirectory();
}

SamplePool::~SamplePool()
{
  clear();
}

//-------------------------------------------------------------------------------------------------
// setup:

void SamplePool::setSampleDirectory(const File &newSampleDirectory)
{
  audioFileBuffers.getLock().enter();
  for(int i=0; i<audioFileBuffers.size(); i++)
  {
    String relativePath 
      = audioFileBuffers[i]->getUnderlyingFile().getRelativePathFrom(sampleDirectory);
    String newPath = newSampleDirectory.getFullPathName() + File::separatorString + relativePath;
    audioFileBuffers[i]->filePathChanged(File(newPath));
  }
  audioFileBuffers.getLock().exit();
  sampleDirectory = newSampleDirectory;
}

int SamplePool::addIfNotAlreadyThere(AudioFileBuffer *bufferToAdd)
{
  audioFileBuffers.getLock().enter();
  bufferToAdd->acquireReadLock();

  int indexInPool = getAudioFileBufferIndexInPool(bufferToAdd);

  // if the sample was not already in the pool, we create a new AudioFileBuffer, add it to the 
  // pool:
  if( indexInPool == -1 )
  {
    File sourceFile = bufferToAdd->getUnderlyingFile();
    File targetFile;

    // if the source file already resides somewhere inside the project's sample directory we use 
    // that existing file:
    if( sourceFile.isAChildOf(sampleDirectory) )
      targetFile = sourceFile;
    else
    {
      // copy the underlying file into the sample directory, if not already there and enable write 
      // access for the copy (which we may or may not already have):
      targetFile = File( 
        sampleDirectory.getFullPathName() + File::separatorString + sourceFile.getFileName() );
      bool fileWasCopied = sourceFile.copyFileTo(targetFile);
      if( !fileWasCopied )
      {
        showFileCouldNotBeCopiedBox(sourceFile.getFileName(),        
          targetFile.getParentDirectory().getFullPathName());
      }
      else
        targetFile.setReadOnly(false, false);
    }

    // create a new buffer and add it to the array:
    AudioFileBuffer* newBufferToAdd = new AudioFileBuffer(targetFile);
    audioFileBuffers.add(newBufferToAdd);

    indexInPool = audioFileBuffers.size()-1; // the new buffer was appended at the end
  }

  bufferToAdd->releaseReadLock();
  audioFileBuffers.getLock().exit();
  return indexInPool;
}

int SamplePool::findFileInPool(const File& fileToCheckFor)
{
  int result = -1;
  audioFileBuffers.getLock().enter();
  for(int i=0; i<audioFileBuffers.size(); i++)
  {
    if( fileToCheckFor == audioFileBuffers[i]->getUnderlyingFile() )
      result = i;
  }
  audioFileBuffers.getLock().exit();
  return result;
}

bool SamplePool::renameSampleFile(const File&  fileToRename, const File&  targetFile)
{
  if( fileToRename.isDirectory() )
    return renameSampleDirectory(fileToRename, targetFile);
  else
    return moveSampleFile(fileToRename, targetFile);
}

bool SamplePool::renameSampleDirectory(const File &directoryToRename, const File &targetDirectory)
{
  return moveDirectory(directoryToRename, targetDirectory);
}

bool SamplePool::moveSampleFileInto(const File& fileToMove, const File& targetDirectory)
{
  if( fileToMove.isDirectory() )
    return moveDirectoryInto(fileToMove, targetDirectory);
  else
  {
    File targetFile 
      = File(targetDirectory.getFullPathName() + File::separatorString + fileToMove.getFileName());
    return moveSampleFile(fileToMove, targetFile);
  }
}

bool SamplePool::moveDirectoryInto(const File &directoryToMove, const File &targetDirectory)
{
  if( !targetDirectory.exists() )
    targetDirectory.createDirectory();
  
  Array<File> filesInSourceDirectory; 
  directoryToMove.findChildFiles(filesInSourceDirectory, File::findFiles, true);
  bool fileWasMoved = false;
  File   oldFile, newFile;
  String oldFileRelativePath;
  for(int i=0; i<filesInSourceDirectory.size(); i++)
  {
    oldFile = filesInSourceDirectory[i];
    oldFileRelativePath = oldFile.getRelativePathFrom(directoryToMove.getParentDirectory());
    newFile 
      = File(targetDirectory.getFullPathName() + File::separatorString + oldFileRelativePath);

    if( !newFile.getParentDirectory().exists() )
      newFile.getParentDirectory().createDirectory();
    fileWasMoved = oldFile.moveFileTo(newFile);
    if( fileWasMoved )
    {
      int index = findFileInPool(oldFile);
      sendSamplePathChangeNotification(index, oldFile, newFile);
    }
    else
    {
      showFileCouldNotBeMovedBox(oldFile.getFileName(), 
                                 newFile.getParentDirectory().getFullPathName());
    }
  }

  // if all the files could be moved the directory should not contain any files anymore (albeit
  // possibly empty subdirectories) - in this case, we can now delete the old directory and report
  // success, otherwise we leave the directory there and report failure:
  if( !hasDirectoryFiles(directoryToMove) )
  {
    directoryToMove.deleteRecursively();
    return true;
  }
  else
    return false;
}

void SamplePool::removeSampleFromPool(const File& fileToRemove, bool deletePhysically)
{
  if( fileToRemove.isDirectory() )
    removeDirectoryFromPool(fileToRemove, deletePhysically);

  audioFileBuffers.getLock().enter();
  clients.getLock().enter();

  int indexInPool = findFileInPool(fileToRemove);
  if( indexInPool != -1 )
  {
    sendSampleToBeDeletedNotification(indexInPool);

    // we now assume that all our clients have properly cleared their references to the sample that 
    // is to be removed - we now actually remove it and delete the object from the heap:
    audioFileBuffers.remove(indexInPool, true);
  }

  // if so chosen, we also delete the underlying file from the harddisc::
  if( deletePhysically == true )
  {
    fileToRemove.setReadOnly(false, true);
    bool fileWasDeleted = fileToRemove.deleteFile();
    if( !fileWasDeleted )
      showFileCouldNotBeDeletedBox(fileToRemove.getFileName());
  }

  clients.getLock().exit();
  audioFileBuffers.getLock().exit();
}

void SamplePool::removeDirectoryFromPool(const File& directoryToRemove, bool deletePhysically)
{
  audioFileBuffers.getLock().enter();
  clients.getLock().enter();

  Array<File> filesToRemove; 
  directoryToRemove.findChildFiles(filesToRemove, File::findFiles, true);
  File fileToRemove;
  for(int i=0; i<filesToRemove.size(); i++)
  {
    fileToRemove    = filesToRemove[i];
    int indexInPool = findFileInPool(fileToRemove);
    if( indexInPool == -1 )
    {
      // we don't delete a file when it is not currently in visible in the pool (i.e. not loaded 
      // into the RAM - do nothing
    }
    else
    {
      sendSampleToBeDeletedNotification(indexInPool);
      audioFileBuffers.remove(indexInPool, true);
      if( deletePhysically == true )
      {
        bool fileWasDeleted = fileToRemove.deleteFile();
        if( !fileWasDeleted )
          showFileCouldNotBeDeletedBox(fileToRemove.getFileName());
      }
    }
  }

  // if the directory is empty now (except for empty subdirectories) - delete the directory (and
  // its subdirectories) itself:
  if( !hasDirectoryFiles(directoryToRemove) )
    directoryToRemove.deleteRecursively();

  clients.getLock().exit();
  audioFileBuffers.getLock().exit();
}

void SamplePool::clear()
{
  audioFileBuffers.getLock().enter();
  audioFileBuffers.clear(true);
  audioFileBuffers.getLock().exit();
}

void SamplePool::addClient(SamplePoolClient *clientToAdd)
{
  clients.getLock().enter();
  clients.addIfNotAlreadyThere(clientToAdd);
  clients.getLock().exit();
}

void SamplePool::removeClient(SamplePoolClient *clientToRemove)
{
  clients.getLock().enter();
  clients.removeValue(clientToRemove);
  clients.getLock().exit();
}

//-------------------------------------------------------------------------------------------------
// inquiry:

int SamplePool::getAudioFileBufferIndexInPool(const AudioFileBuffer* bufferToLookFor) const
{
  int result = -1;

  audioFileBuffers.getLock().enter();
  for(int i=0; i<audioFileBuffers.size(); i++)
  {
    if( *(audioFileBuffers[i]) == *bufferToLookFor ) 
    {
      result = i;
      break;
    }
  }
  audioFileBuffers.getLock().exit();

  return result;
}

AudioFileBuffer* SamplePool::getAudioFileBuffer(int index)
{
  if( index >= 0 && index < audioFileBuffers.size() )
    return audioFileBuffers[index];
  else
    return NULL;
}

const File& SamplePool::getSampleDirectory() const
{
  return sampleDirectory;
}

//-------------------------------------------------------------------------------------------------
// others:

void SamplePool::lockAudioFileBufferArray()
{
  audioFileBuffers.getLock().enter();
}

void SamplePool::unlockAudioFileBufferArray()
{
  audioFileBuffers.getLock().exit();
}

//-------------------------------------------------------------------------------------------------
// internal functions:

bool SamplePool::moveSampleFile(const File& oldFile, const File& newFile)
{
  if( newFile == oldFile )
    return true; // nothing to do

  if( newFile.existsAsFile() )
  {
    // issue a warning box that the file could not be moved because at in the target directory 
    // already exists a file with the same name:
    showFileCouldNotBeRenamedBox(oldFile.getFullPathName(), newFile.getFullPathName());
    return false;
  }

  bool fileWasMoved;
  int  index = findFileInPool(oldFile);
  if( index == -1 )
  {
    // file is present on the harddisc but not not in the current pool (inside the RAM) - but we 
    // want to move the file nonetheless:
    fileWasMoved = oldFile.moveFileTo(newFile);
    if( fileWasMoved )
      return true;
    else
    {
      showFileCouldNotBeRenamedBox(oldFile.getFullPathName(), newFile.getFullPathName());
      return false;
    }
  }
  else
  {
    audioFileBuffers.getLock().enter();
    fileWasMoved = oldFile.moveFileTo(newFile);
    if( fileWasMoved == true ) 
    {
      sendSamplePathChangeNotification(index, oldFile, newFile);
    }
    else 
    {
      jassertfalse;   // for some reason the file could not be moved
      audioFileBuffers.getLock().exit();
      showFileCouldNotBeRenamedBox(oldFile.getFullPathName(), newFile.getFullPathName());
      return false;
    }
    audioFileBuffers.getLock().exit();
    return true;
  }
}

bool SamplePool::moveDirectory(const File& oldDirectory, const File& newDirectory)
{
  if( !oldDirectory.exists() || !oldDirectory.isDirectory() )
    return false;
  if( !newDirectory.exists() )
    newDirectory.createDirectory();

  Array<File> filesInSourceDirectory; 
  oldDirectory.findChildFiles(filesInSourceDirectory, File::findFiles, true);
  bool fileWasMoved = false;
  File   oldFile, newFile;
  String oldFileRelativePath;
  for(int i=0; i<filesInSourceDirectory.size(); i++)
  {
    oldFile             = filesInSourceDirectory[i];
    oldFileRelativePath = oldFile.getRelativePathFrom(oldDirectory);
    newFile 
      = File(newDirectory.getFullPathName() + File::separatorString + oldFileRelativePath);

    if( !newFile.getParentDirectory().exists() )
      newFile.getParentDirectory().createDirectory();
    fileWasMoved = oldFile.moveFileTo(newFile);
    if( fileWasMoved )
    {
      int index = findFileInPool(oldFile);
      sendSamplePathChangeNotification(index, oldFile, newFile);
    }
    else
    {
      showFileCouldNotBeMovedBox(oldFile.getFileName(), 
                                 newFile.getParentDirectory().getFullPathName());
    }
  }

  // if all the files could be moved the directory should not contain any files anymore (albeit
  // possibly empty subdirectories) - in this case, we can now delete the old directory and report
  // success, otherwise we leave the directory there and report failure:
  if( !hasDirectoryFiles(oldDirectory) )
  {
    oldDirectory.deleteRecursively();
    return true;
  }
  else
    return false;
}

void SamplePool::sendSamplePathChangeNotification(int index, const File& oldPath, 
                                                  const File& newPath)
{
  if( index < 0 || index >= audioFileBuffers.size() )
    return;
  audioFileBuffers[index]->filePathChanged(newPath);
  clients.getLock().enter();
  for(int c=0; c<clients.size(); c++)
    clients[c]->samplePathHasChanged(index, oldPath, newPath);
  clients.getLock().enter();
}

void SamplePool::sendSampleToBeDeletedNotification(int index)
{
  clients.getLock().enter();
  for(int c=0; c<clients.size(); c++)
    clients[c]->sampleIsToBeDeleted(index);
  clients.getLock().enter();
}
