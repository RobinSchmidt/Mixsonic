#include "SamplePoolClient.h"

//-------------------------------------------------------------------------------------------------
// construction/destruction:

SamplePoolClient::SamplePoolClient(SamplePool *samplePoolToUse) 
{
  samplePool = samplePoolToUse;
  if( samplePool != NULL )
    samplePool->addClient(this);
}

SamplePoolClient::~SamplePoolClient()
{
  if( samplePool != NULL )
    samplePool->removeClient(this);
}
