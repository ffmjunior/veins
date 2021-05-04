#include "FogNode.h"

FogNode::FogNode () :
    FogNode (10000)
{
}

FogNode::FogNode (unsigned long int processingCapacity)
{
  this->setProcessingCapacity (processingCapacity);
}

FogNode::FogNode (unsigned long int processingCapacity,
		  long applicationTypeMean, long applicationTypeDeviation) :
    FogNode (processingCapacity)
{
  this->setProcessingCapacity (processingCapacity);
  this->createApplicationSet (applicationTypeMean, applicationTypeDeviation);

}

fogApplicationQueue
FogNode::getAppQueue ()
{
  return applicationQueue;
}

bool
FogNode::hasApplication (int applicationId)
{
//return applicationStorage->hasApplication(applicationId);
  return true;
}

//new

double
FogNode::newTask (FogMessage fogMessage, double timestamp)
{
  FogApplicationInstance newTask (timestamp, fogMessage.getLoad (),
				  fogMessage.getUUID ());
  double estimatedDuration = newTask.getLoad () / this->capacity;
  newTask.setCompletionTime (estimatedDuration);
  newTask.info ();
  if (this->applicationQueue.empty ())
    {
      this->applicationQueue.push_back (newTask);
      return estimatedDuration;
    }
  else
    {
      this->applicationQueue.push_back (newTask);
      estimatedDuration = this->getQueueDuration ();
    }
  return estimatedDuration;
}

double
FogNode::getQueueDuration ()
{

  double queueDuration = 0.0;
  fogApplicationQueue copyQueue (applicationQueue);
  while (!copyQueue.empty ())
    {
      FogApplicationInstance instance = copyQueue.front ();
      queueDuration = queueDuration + instance.getDuration ();
      copyQueue.pop_front ();
    }
  return queueDuration;
}

void
FogNode::addToRequestQueue (FogMessage request)
{
}

void
FogNode::calculateResponse ()
{
}

FogMessage
FogNode::getResponse ()
{
  FogMessage response;
  FogApplicationInstance app = this->applicationQueue.front ();
  response.setUUID (app.getUUID ());
  this->applicationQueue.pop_front ();
  return response;
}

//get the tail from the request queue
void
FogNode::addToApplicationQueue ()
{
}

void
FogNode::sendResponse ()
{
}

double
FogNode::cancelCurrentTask ()
{
//remove current front task
//send new estimate from second task
// there is a queue of tasks already notified to cancel
  return 0.0;    // next task schedule

}

void
FogNode::setProcessingCapacity (unsigned long int processingCapacity)
{
//this->setProcessingCapacity(processingCapacity, processingCapacity / 2);
  this->setProcessingCapacity (processingCapacity, 1);
}

int
FogNode::getQueueSize ()
{
  return this->applicationQueue.size ();
}

double
FogNode::getCurrentTaskDuration ()
{
  double duration = 0.0;
  if (applicationQueue.size () > 0)
    {
      FogApplicationInstance app = this->applicationQueue.front ();
      app.info ();
      duration = app.getDuration ();
    }
  return duration;
}

void
FogNode::createApplicationSet (long applicationTypeMean,
			       long applicationTypeDeviation)
{
  long start = applicationTypeMean - applicationTypeDeviation;
  long end = applicationTypeMean + applicationTypeDeviation;
  while (start <= end)
    {
      applicationTypeSet.insert (start);
      start = start + 1;
    }
}

double
FogNode::checkApplicationType (long applicationType)
{
  return applicationTypeSet.count (applicationType);
}

double
FogNode::getTaskDuration (std::string uuid)
{
  double jobDuration = 0.0;
  fogApplicationQueue copyQueue (applicationQueue);
  std::stringstream ss;
  while (!copyQueue.empty ())
    {
      FogApplicationInstance instance = copyQueue.front ();
      jobDuration = jobDuration + instance.getDuration ();
      copyQueue.pop_front ();
      if (uuid.compare (instance.getUUID ()) == 0)
	{
	  return jobDuration;
	}
    }
  return jobDuration;
}

double
FogNode::checkNotification (FogMessage notification)
{
//std::cout << "CHECK NOTIFICATION " << notification.info() << std::endl;
  return this->getTaskDuration (notification.getUUID ());
}

bool
FogNode::cancelTask (std::string uuid)
{
  if (applicationQueue.size () > 0)
    {
      fogApplicationQueue::iterator it;
      for (it = applicationQueue.begin (); it != applicationQueue.end (); it++)
	{
	  if (uuid == (*it).getUUID ())
	    {
	      this->applicationQueue.erase (it);
	      return true;
	    }
	}
    }
  return false;
}

double
FogNode::checkApplicationUUID (FogApplicationInstance instance,
			       std::string uuid)
{
  if (instance.getUUID () == (uuid))
    {
      return true;
    }
  else
    {
      return false;
    }
}

FogApplicationInstance
FogNode::getCurrentTask ()
{
  FogApplicationInstance app;
  if (applicationQueue.size () > 0)
    {
      app = this->applicationQueue.front ();
    }
  return app;
}

double
FogNode::getCapacity ()
{
  return this->capacity;
}

void
FogNode::setProcessingCapacity (unsigned long int processingCapacity,
				unsigned long int variation)
{
  this->capacity = FogStatistics::randomNormal (processingCapacity, variation);
}

FogNode::~FogNode ()
{
//    std::cout << "DESTRUCTOR FN " << std::endl;
//    this->requestQueue.~list();
//    std::cout << "DESTRUCTOR FN " << std::endl;
//    this->applicationQueue.~list();
//    std::cout << "DESTRUCTOR FN " << std::endl;
//    this->applicationTypeSet.~set();
//    std::cout << "DESTRUCTOR FN " << std::endl;
}
