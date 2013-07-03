#include "DeletionManagement.h"

DeletionRequester::DeletionRequester(DeletionManager *deletionManagerToUse)
{
  deletionManager = deletionManagerToUse;
}

void DeletionRequester::setDeletionManager(DeletionManager *newDeletionManager)
{
  deletionManager = newDeletionManager;
}

 DeletionManager* DeletionRequester::getDeletionManager()
 {
   return deletionManager;
 }

 void DeletionRequester::requestDeletion()
 {
   jassert(deletionManager != nullptr);
   if( deletionManager != nullptr )
     deletionManager->handleDeletionRequest(this);
 }

//================================================================================================= 

 void DeletionManager::handleDeletionRequest(DeletionRequester* objectThatWantsToBeDeleted)
 {
   delete objectThatWantsToBeDeleted;
 }
