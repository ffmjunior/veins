
#include "FogApplication.h"
#include "FogApplicationInstance.h"
#include "FogMessage.h"
#include "FogStatistics.h"

#include <list>
#include <math.h>
#include <mutex>
#include <queue>
#include <set>

//required for compatibility
typedef std::map<std::string, FogMessage> fogMap;
typedef std::list<FogMessage> fogMessageQueue;
typedef std::list<FogApplicationInstance> fogApplicationQueue;
typedef std::set<int> fogApplicationTypeSet;


/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 * @author ffmjunior@hotmail.com
 */
class FogNode
{

private:
  int capacity = 0;
  int timeRemaining = 0;
  int carId = 0;

  //heterogeneity
  long applicationType;
  long applicationTypeMean = 100;
  long applicationTypeDeviation = 10;

  // operation
  //RequestQueue *requestQueue;
  fogMessageQueue requestQueue; // named queue, but it ir actually a list
  fogApplicationQueue applicationQueue;
  fogApplicationTypeSet applicationTypeSet;

public:
  FogNode ();
  FogNode (unsigned long int processingCapacity);
  FogNode (unsigned long int processingCapacity, long applicationTypeMean,
	   long applicationTypeDeviation);
  virtual fogApplicationQueue
  getAppQueue ();
  virtual bool
  hasApplication (int applicationId);
  ~FogNode ();

  //getters and setters
  virtual void
  addToApplicationQueue (); //get the tail from the request queue
  virtual void
  addToRequestQueue (FogMessage request);
  virtual void
  calculateResponse ();
  virtual double
  cancelCurrentTask ();
  virtual bool
  cancelTask (std::string uuid);
  virtual double
  checkNotification (FogMessage notification);
  virtual double
  checkApplicationType (long applicationType);
  virtual double
  checkApplicationUUID (FogApplicationInstance instance, std::string uuid); //helper to remove task
  virtual void
  createApplicationSet (long applicationTypeMean,
			long applicationTypeDeviation);
  virtual double
  getCurrentTaskDuration ();
  virtual FogApplicationInstance
    getCurrentTask ();
  virtual double
    getCapacity();
  virtual double
  getTaskDuration (std::string uuid);
  virtual double
  getQueueDuration ();
  virtual int
  getQueueSize ();
  virtual FogMessage
  getResponse ();
  virtual double
  newTask (FogMessage fogMessage, double timestamp);
  virtual void
  sendResponse ();
  virtual void
  setProcessingCapacity (unsigned long int processingCapacity);
  virtual void
  setProcessingCapacity (unsigned long int processingCapacity,
			 unsigned long int variation);

  int
  getCarId () const
  {
    return carId;
  }

  void
  setCarId (int carId = 0)
  {
    this->carId = carId;
  }
};
