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

#ifndef TraCIDemo11p_H
#define TraCIDemo11p_H

#include "veins/modules/application/ieee80211p/BaseWaveApplLayer.h"

#include "fogwise/FogMessage.h"
#include "fogwise/FogStatistics.h"
#include "fogwise/FogNode.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <map>
#include <sstream>
#include <string>
#include <vector>

//required for compatibility
typedef std::map<std::string, FogMessage> fogMap;
typedef std::set<int> carIdSet;

//logging
// std::stringstream logSink;
//#define LOG_FIXED logSink.str(""); logSink <<  "" // std::cout << "" // logSink << ""
//#define LOG_DEBUG logSink.str(""); std::cout << "[F" << myId << " TEST]   " // std::cout << "[TEST]   " // logSink << ""
#define noop

/**
 * @brief
 * A tutorial demo for TraCI. When the car is stopped for longer than 10 seconds
 * it will send a message out to other cars containing the blocked road id.
 * Receiving cars will then trigger a reroute via TraCI.
 * When channel switching between SCH and CCH is enabled on the MAC, the message is
 * instead send out on a service channel following a WAVE Service Advertisement
 * on the CCH.
 *
 * @author Christoph Sommer : initial DemoApp
 * @author David Eckhoff : rewriting, moving functionality to BaseWaveApplLayer, adding WSA
 *
 */

class FogWiseFull : public BaseWaveApplLayer
{
public:
  virtual void
  initialize (int stage);

protected:
  //common
  simtime_t lastDroveAt;
  bool sentMessage;
  int currentSubscribedServiceId;
  bool isClient;
  static int count;
  static int lastId;
  int clientRatio = 2; //use this to set the ratio of clients. Ex.: 2 -> 1/2 clients; 3 -> 1/3 clients and so on...
  int serverRatio = 1; //same for server ratio. One of them must be = 1

  //server
  FogNode fogNode;
  cMessage *respondRequestEvt;

  //client operation
  fogMap requestMap;

  //server parameters
  long processingCapacity = 2000;
  long processingCapacityDeviation = 500;
  long applicationTypeMean = 100;
  long applicationTypeDeviation = 5;
  long applicationType;
  int notificationMethod = 1; // use 0-no method; 1-notification; 2-remoteResponse
  int decisionMethod = 0; // use 0-processing time; 1-distance;
  double backoffStart = 0.01;
  double backoffEnd = 0.1;

  //client parameters
  double applicationLoadMean = 1100.0;
  double applicationLoadDeviation = 100.0;
  int requestTypeMean = 100;
  int requestTypeDeviation = 5;
  double requestSizeMean = 0.0;
  double requestSizeDeviation = requestSizeMean / 10.0;

  // common metrics
  carIdSet ids; // all interactions of a vehicle
  carIdSet temporalIds; // counts timely interactions. That is, each second, feeds interactionVector and interactionStats
  cOutVector interactionVector; // timely interactions between vehicles based on the messages received
  cLongHistogram interactionStats; // histogram of interactions based on IDs of messages received
  int countTemporalMessages = 0; // counts and resets messages every second
  cOutVector messageRatesVector;
  cLongHistogram messageRatesStats;

  //client metrics
  double averageRtt = 0.0;
  int requestsGenerated = 0;
  int responsesReceived = 0;
  cOutVector responseHistoryVector; //stores the types of responses received. Use to evaluate the percentage of requests fulfilled
  cLongHistogram responseHistoryStats; // histogram of request types on the responses.
  int requestsOverhead = 0; // responses received from more than one server. That is, duplicated responses.
  int requestsFromOthers = 0; // responses destined to other vehicles but arrived.
  cLongHistogram requestTypeStats; // histogram of message types on the generation of requests
  cDoubleHistogram requestLoadStats; // histogram of loads on the generation of requests
  cOutVector requestTypeVector; //vector of types of messages generated

  //server metrics
  int requestsReceived = 0;
  int requestsDiscarded = 0;
  int requestsAccepted = 0;
  cOutVector requestHistory; // stores requests. Might remove
  int notificationsSent = 0; //notifications from method 1
  int notificationsReceived = 0;
  int tasksAccepted = 0;
  int tasksFinished = 0;
  int tasksCancelled = 0;
  int responsesSent = 0;
  int queueSize = 0; // feeds queueSizesVector
  double queueDuration = 0.0; //feeds queueDurationsVector
  cOutVector queueDurationsVector; // timeline of queue durations. Updates each time the vehicle accepts a request
  cDoubleHistogram queueDurationStats; // histogram of queue durations. Need to fix to match the behaviour of interaction metrics
  cOutVector queueSizeVector; // timeline of queue sizes. Updates each time the vehicle accepts a request
  cLongHistogram queueSizeStats;

protected:
  virtual void
  onWSM (WaveShortMessage *wsm);
  virtual void
  onWSA (WaveServiceAdvertisment *wsa);
  virtual void
  handleSelfMsg (cMessage *msg);
  virtual void
  handlePositionUpdate (cObject *obj);

  // server
  virtual void
  handleRequest (WaveShortMessage *wsm);
  virtual void
  handleNotification (WaveShortMessage *wsm);
  virtual void
  handleRemoteResponse (WaveShortMessage *wsm);
  virtual void
  notifyRequest (std::string uuid, double estimatedDuration, double distance);
  virtual void
  notifyResponse ();
  virtual void
  calculateResponse ();
  virtual void
  sendResponse ();

  //common
  virtual void
  sendWSM (FogMessage message, int psid = 0, double delay = 0);
  virtual void
  finish ();

  //client
  virtual void
  generateRequest (int type, double requirement);
  virtual void
  handleResponse (WaveShortMessage *wsm);

};

#endif
