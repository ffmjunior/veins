//
// Copyright (C) 2006-2011 Christoph Sommer <christoph.sommer@uibk.ac.at>
//
// Documentation for these modules is at http://veins.car2x.org/
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include "veins/modules/application/traci/FogWiseFull.h"

Define_Module(FogWiseFull);

int FogWiseFull::count = 0;
int FogWiseFull::lastId = -1;

void
FogWiseFull::initialize (int stage)
{
  BaseWaveApplLayer::initialize (stage);
  std::cout << "CAR ID " << myId << " COUNT " << FogWiseFull::count
      << std::endl;
  if (FogWiseFull::lastId != myId)
    { //small hack required to create correct ratios of clients and servers once veins internally creates two nodes to each car
      FogWiseFull::count = FogWiseFull::count + 1;
      FogWiseFull::lastId = myId;
    }

  //read parameters
  clientRatio = par ("clientRatio");
  serverRatio = par ("serverRatio");
  processingCapacity = par ("processingCapacity");
  processingCapacityDeviation = par ("processingCapacityDeviation");
  applicationLoadMean = par ("applicationLoadMean");
  applicationLoadDeviation = par ("applicationLoadDeviation");
  applicationTypeMean = par ("applicationTypeMean");
  applicationTypeDeviation = par ("applicationTypeDeviation");
  requestTypeMean = par ("requestTypeMean");
  requestTypeDeviation = par ("requestTypeDeviation");
  notificationMethod = par ("notificationMethod");
  backoffStart = par ("backoffStart");
  backoffEnd = par ("backoffEnd");
  requestSizeMean = par ("requestSizeMean");
  requestSizeDeviation = par ("requestSizeDeviation");

  if (this->clientRatio > this->serverRatio)
    {
      if (FogWiseFull::count % this->clientRatio == 0)
	{
	  this->isClient = true;
	}
      else
	{
	  this->isClient = false;
	}
    }
  else
    {
      if (FogWiseFull::count % this->serverRatio == 0)
	{
	  this->isClient = false;
	}
      else
	{
	  this->isClient = true;
	}
    }

  if (stage == 0)
    {
      sentMessage = false;
      lastDroveAt = simTime ();
      currentSubscribedServiceId = -1;
    }

  if (isClient)
    {
      std::cout << "CLIENT " << std::endl;
      fogNode.setProcessingCapacity (0);
    }
  else
    {
      std::cout << "SERVER " << std::endl;
      respondRequestEvt = new cMessage ("respond request evt", 13);
      cancelEvent (respondRequestEvt);

      fogNode.setProcessingCapacity (processingCapacity,
				     processingCapacityDeviation);
      fogNode.createApplicationSet (applicationTypeMean,
				    applicationTypeDeviation);
      fogNode.setCarId (myId);
      requestHistory.disable ();
      queueSizeVector.disable ();
      interactionVector.disable ();
      responseHistoryVector.disable ();
    }

}

void
FogWiseFull::onWSA (WaveServiceAdvertisment *wsa)
{
// kept as default
  if (currentSubscribedServiceId == -1)
    {
      mac->changeServiceChannel (wsa->getTargetChannel ());
      currentSubscribedServiceId = wsa->getPsid ();
      if (currentOfferedServiceId != wsa->getPsid ())
	{
	  stopService ();
	  startService ((Channels::ChannelNumber) wsa->getTargetChannel (),
			wsa->getPsid (), "Mirrored Traffic Service");
	}
    }
}

void
FogWiseFull::onWSM (WaveShortMessage *wsm)
{
  //metric
  int senderAddress = wsm->getSenderAddress ();
  ids.emplace (senderAddress);
  temporalIds.emplace (senderAddress);
  countTemporalMessages = countTemporalMessages + 1;
  //messageRatesStats.re
  // std::cout << "WSM " << wsm->getPsid () << " RECEIVED " << wsm->getWsmData ();
  if (!interactionVector.isEnabled ())
    {
      interactionVector.setName ("interactionVector");
      interactionVector.enable ();
    }
  if (!messageRatesVector.isEnabled ())
    {
      messageRatesVector.setName ("messageRatesVector");
      messageRatesVector.enable ();
    }
  if (isClient)
    {

      switch (wsm->getPsid ())
	{
	case 101:
	  // ignore
	  //delete (wsm);
	  break;
	case 102:
	  // ignore
	  //delete (wsm);
	  break;
	case 103:
	  // fullfillment and statistics
	  this->handleResponse (wsm->dup ());
	  break;
	}
    }
  else
    { //server
      switch (wsm->getPsid ())
	{
	case 101:
	  //metric
	  requestsReceived = requestsReceived + 1;
	  if (!requestHistory.isEnabled ())
	    {
	      requestHistory.setName ("requestHistory");
	      requestHistory.enable ();
	    }

	  if (!queueSizeVector.isEnabled ())
	    {
	      queueSizeVector.setName ("queueSizeVector");
	      queueSizeVector.enable ();
	    }

	  if (!responseHistoryVector.isEnabled ())
	    {
	      responseHistoryVector.setName ("responseHistoryVector");
	      responseHistoryVector.enable ();
	    }
	  requestHistory.recordWithTimestamp (simTime (),
					      wsm->getSenderAddress ());
	  //add application to queue and notifies all other vehicles
	  handleRequest (wsm->dup ());
	  break;
	case 102:
	  // receives notifications and decides to keep or withdraw the request
	  //metric
	  notificationsReceived = notificationsReceived + 1;
	  if (notificationMethod == 1)
	    {
	      handleNotification (wsm->dup ());
	    }
	  break;
	  break;
	case 103:
	  //Ignore or discard current or scheduled jobs
	  responsesReceived = responsesReceived + 1;
	  if (notificationMethod == 2)
	    {
	      this->handleRemoteResponse (wsm);
	    }
	  break;
	}
    }
}

void
FogWiseFull::handleSelfMsg (cMessage *msg)
{
  if (isClient)
    {
      switch (msg->getKind ())
	{
	case SEND_BEACON_EVT: //client
	  //std::cout << "Self Message " << simTime().dbl() << std::endl;
	  //crie e envia a requisição
	  //std::cout << "RSU POSITION " << mobility->getCurrentPosition().info() << std::endl;
	  generateRequest (10, 50);
	  scheduleAt (simTime () + beaconInterval, sendBeaconEvt);
	  break;
	}
    }
  else //server self message
    {
      switch (msg->getKind ())
	{ //server
	case 13:
//	    std::cout << myId << " EVENT " << msg->getName() << std::endl;
	  this->sendResponse ();
	  break;
	case 14:
	  //this->sendResponse();
	  noop
	  break;
	}
    }

}

void
FogWiseFull::handlePositionUpdate (cObject *obj)
{
  //std::cout << myId << " POSITION UPDATE " << simTime () << std::endl;
  queueSizeStats.collect (fogNode.getQueueSize ());
  interactionVector.record (temporalIds.size ());
  //interactionVector.recordWithTimestamp (simTime (), temporalIds.size ());
  interactionStats.collect (temporalIds.size ());
  temporalIds.clear ();

  messageRatesVector.record (countTemporalMessages);
  messageRatesStats.collect (countTemporalMessages);

  countTemporalMessages = 0;
}

//new

void
FogWiseFull::handleRequest (WaveShortMessage *wsm)
{
  FogMessage fogMessage (wsm->getWsmData ());
  if (fogNode.checkApplicationType (fogMessage.getApplicationType ()))
    {
      requestsAccepted = requestsAccepted + 1;
      simtime_t requestTime = simTime ();
      double estimatedDuration = fogNode.newTask (fogMessage,
						  requestTime.dbl ());
      //metric
      queueDurationStats.collect (estimatedDuration);
      //queueSizeStats.collect (fogNode.getQueueSize ());
      if (!respondRequestEvt->isScheduled ())
	{
	  cancelEvent (respondRequestEvt);
	  respondRequestEvt->setName (fogMessage.getUUID ().c_str ());
	  scheduleAt (simTime () + estimatedDuration, respondRequestEvt);
	  //metric
	  this->tasksAccepted = this->tasksAccepted + 1;
	}
      if (notificationMethod == 1)
	{
	  this->notifyRequest (
	      fogMessage.getUUID (),
	      estimatedDuration,
	      mobility->getCurrentPosition ().distance (
		  fogMessage.getCoordinates ()));
	}
    }
  else
    {
//std::cout << "REQUEST NOT ACCEPTED " << std::endl;
      this->requestsDiscarded = this->requestsDiscarded + 1;
    }
  delete (wsm);
}

void
FogWiseFull::handleNotification (WaveShortMessage *wsm)
{
  std::string eventName;
  std::string notificationName;
  FogMessage notification (wsm->getWsmData ());

  if (decisionMethod == 0) // response time
    {
      double estimate = fogNode.checkNotification (notification);
      if (estimate > notification.getEstimatedProcessing ())
	{
	  if (fogNode.cancelTask (notification.getUUID ()))
	    {
	      //metric
	      this->tasksCancelled = this->tasksCancelled + 1;
	      eventName = respondRequestEvt->getName ();
	      notificationName = notification.getUUID ();
	      if (eventName == notificationName)
		{
		  cancelEvent (respondRequestEvt);
		  FogApplicationInstance currentJob = fogNode.getCurrentTask ();
		  if (currentJob.getDuration () > 0)
		    {
		      respondRequestEvt->setName (
			  currentJob.getUUID ().c_str ());
		      scheduleAt (simTime () + currentJob.getDuration (),
				  respondRequestEvt);
		    }
		}
	    }
	}
    }

  if (decisionMethod == 1)
    {
  noop
  // double distance = notification.getDistance ();
  // distance = mobility->getCurrentPosition ().distance ( fogMessage.getCoordinates ()) );
  //distance = notification.getDistance ();
}

}

/**
 * Cria uma mensagem do tipo 102
 */
void
FogWiseFull::notifyRequest (std::string uuid, double estimatedDuration,
			    double distance)
{
int psid = 102;
FogMessage notification;
notification.setUUID (uuid);
notification.setEstimatedProcessing (estimatedDuration);
notification.setDistance (distance);
double randomBackoff = uniform (0.001, 0.1);
this->sendWSM (notification, psid, randomBackoff);
//metric
notificationsSent = notificationsSent + 1;
}

void
FogWiseFull::notifyResponse ()
{
FogMessage response = fogNode.getResponse ();
this->sendWSM (response, 103, 0);
}

void
FogWiseFull::calculateResponse ()
{

}

void
FogWiseFull::sendResponse ()
{
  //metric

this->tasksFinished = this->tasksFinished + 1;
FogMessage response = fogNode.getResponse ();
//double randomBackoff = uniform (0.001, 0.01);
this->sendWSM (response, 103);
responsesSent = responsesSent + 1;
// nextSchedule
cancelEvent (respondRequestEvt);
double currentJob = fogNode.getCurrentTaskDuration ();
FogApplicationInstance nextTask = fogNode.getCurrentTask ();
//std::cout << "CAR " << myId << " ANSWERING AT " << simTime() << " NEXT JOB " << currentJob << " - " << response.info() << std::endl;
if (nextTask.getDuration () != 0)
{
  respondRequestEvt->setName (nextTask.getUUID ().c_str ());
  scheduleAt (simTime () + currentJob, respondRequestEvt);
}
}

void
FogWiseFull::sendWSM (FogMessage message, int psid, double delay)
{
WaveShortMessage *wsm = new WaveShortMessage ();
populateWSM (wsm);
wsm->setPsid (psid);
wsm->setWsmData (message.info ().c_str ());
if (requestSizeMean > 0 ){
    double requestSize = FogStatistics::randomNormal(requestSizeMean, requestTypeDeviation);
    //std::cout << "REQUEST SIZE " << requestSize << std::endl;
    wsm->setByteLength(requestSize);
}
//wsm->setChannelNumber(176);
if (dataOnSch)
{
  startService (Channels::SCH2, psid, "FogWise");
//started service and server advertising, schedule message to self to send later
  scheduleAt (computeAsynchronousSendingTime (0.1, type_SCH), wsm);
}
else
{
//send right away on CCH, because channel switching is disabled
  if (delay > 0)
    {
      sendDelayedDown (wsm, delay);
    }
  else
    {
      sendDown (wsm);
    }
}
}

void
FogWiseFull::generateRequest (int type, double requirement)
{
int appCode = (int) FogStatistics::randomNormal ();
double load = FogStatistics::randomNormal (this->applicationLoadMean,
					   this->applicationLoadDeviation);
FogMessage request (50.0, 1.0, load);
request.setCoordinate (this->mobility->getCurrentPosition ());
request.setTimestamp (simTime ().dbl ());
request.setApplicationType (requestTypeMean, requestTypeDeviation);
//metric
requestLoadStats.collect (request.getLoad ());
requestTypeStats.collect (request.getApplicationType ());
this->requestMap[request.getUUID ()] = request;
this->sendWSM (request, 101, 0);
this->requestsGenerated = requestsGenerated + 1;
//std::cout << "REQUEST GENERATED " << myId << " - " << request.info () << std::endl;
}

void
FogWiseFull::handleRemoteResponse (WaveShortMessage *wsm)
{
FogMessage remoteResponse (wsm->getWsmData ());
if (fogNode.cancelTask (remoteResponse.getUUID ()))
{
  //metric
  this->tasksCancelled = this->tasksCancelled + 1;
  if (respondRequestEvt->getName () == remoteResponse.getUUID ())
    {
//std::cout << "EVENT MATCHES TASK. " << myId << " CANCELLING. " << std::endl;
      cancelEvent (respondRequestEvt);
//schedule next
      FogApplicationInstance currentJob = fogNode.getCurrentTask ();
      if (currentJob.getDuration () > 0)
	{
	  respondRequestEvt->setName (currentJob.getUUID ().c_str ());
	  scheduleAt (simTime () + currentJob.getDuration (),
		      respondRequestEvt);
//	std::cout << "JOB " << currentJob.getUUID() << " SCHEDULED IN "
//		<< currentJob.getDuration() << std::endl;
	}
      else
	{
//	std::cout << "NO JOBS IN QUEUE " << std::endl;
	}
    }
}
//std::cout << myId << " KEEPING TASK. " << std::endl;
}

/**
 * Quando qualquer resposta chega, a RSU deve realizar os devidos processamentos:
 * - adicionar á fila de requisições geradas
 * - monitorar o desempenho das requisições recebidas
 */
void
FogWiseFull::handleResponse (WaveShortMessage *wsm)
{
simtime_t requestTime = simTime ();
FogMessage response (wsm->getWsmData ());
if (requestMap.count (response.getUUID ()) > 0)
{
  FogMessage request = this->requestMap[response.getUUID ()];
  this->requestsOverhead = requestsOverhead + 1;
  if (request.getRTT () == 0)
    { // prevents duplications
      double rtt = requestTime.dbl () - request.getTimestamp ();
      request.setRtt (requestTime.dbl () - request.getTimestamp ());
      this->requestMap[request.getUUID ()] = request;
      this->averageRtt = (this->averageRtt + rtt) / 2;
      this->responsesReceived = responsesReceived + 1;
      responseHistoryVector.recordWithTimestamp (simTime (),
						 request.getApplicationType ());
      responseHistoryStats.collect (request.getApplicationType ());
//std::cout << "RESPONSE RECEIVED " << myId << " FROM " << wsm->getSenderAddress() << " - " << response.info() << std::endl;
    }
}
else
{
  this->requestsFromOthers = requestsFromOthers + 1;
//std::cout << "DUPLICATED RESPONSE RECEIVED " << myId << " FROM "
//	<< wsm->getSenderAddress() << " - " << response.info() << std::endl;
}
delete (wsm);
}

void
FogWiseFull::finish ()
{
BaseWaveApplLayer::finish ();

double rq = requestsGenerated;
double rr = responsesReceived;

double responseRequestRatio = 0;

if (requestsGenerated > 0)
{
  responseRequestRatio = rr / rq;
}

//common
recordScalar ("isClient", isClient);
recordScalar ("notificationsSent", notificationsSent);
recordScalar ("notificationsReceived", notificationsReceived);
recordScalar ("countCars", FogWiseFull::count);
recordScalar ("interactions", ids.size ());
messageRatesStats.recordAs ("messageRatesStats");

//server
recordScalar ("processingCapacity", fogNode.getCapacity ());
recordScalar ("queueSize", fogNode.getQueueSize ());
recordScalar ("queueDuration", fogNode.getQueueDuration ());
recordScalar ("requestsReceived", requestsReceived);
recordScalar ("responsesSent", responsesSent);
recordScalar ("requestsAccepted", requestsAccepted);
recordScalar ("requestsDiscarded", requestsDiscarded);
recordScalar ("tasksAccepted", tasksAccepted);
recordScalar ("tasksFinished", tasksFinished);
recordScalar ("tasksCancelled", tasksCancelled);

//client
recordScalar ("averageRtt", averageRtt);
recordScalar ("requestsGenerated", requestsGenerated);
recordScalar ("responsesReceived", responsesReceived);
recordScalar ("requestsOverhead", (requestsOverhead - responsesReceived));
recordScalar ("requestsFromOthers", requestsFromOthers);
recordScalar ("responseRequestRatio", responseRequestRatio);
requestTypeStats.recordAs ("requestType");
requestLoadStats.recordAs ("requestLoad");
queueDurationStats.recordAs ("queueDuration");
responseHistoryStats.recordAs ("responseHistoryStats");
interactionStats.recordAs ("interactionStats");
//server
if (!isClient)
{
  cancelAndDelete (respondRequestEvt);
}

std::cout << "CAR " << myId << " SHUTTING DOWN" << std::endl;

}
