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
#include <string>
#include <vector>

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

class FogWiseCarClient: public BaseWaveApplLayer {
public:
    virtual void initialize(int stage);
    long int processingCapacity = 1000;
    long applicationTypeMean = 100;
    long applicationTypeDeviation = 5;
    ~FogWiseCarClient();
protected:

    //common
    simtime_t lastDroveAt;
    bool sentMessage;
    int currentSubscribedServiceId;
    bool isClient;
    static int count;
    int clientRatio = 3; //use this to set the ratio of clients. Ex.: 2 -> 1/2 clients; 3 -> 1/3 clients and so on...

    //server
    FogNode fogNode;
    cMessage* respondRequestEvt;

    //metrics
    int queueSize = 0;
    double queueDuration = 0.0;
    int requestsReceived = 0;
    int responsesSent = 0;
    int requestsAccepted = 0;
    cOutVector requestSizes;

    //client
    std::map<std::string, FogMessage> requestQueue;
    //parameters
    double applicationLoadMean = 1100.0;
    double applicationLoadDeviation = 100.0;
    long applicationType;
    //long applicationTypeMean = 100;
    //long applicationTypeDeviation = 5;
    //metrics
    double averageRtt = 0.0;
    int requestsGenerated = 0;
    int requestsMet = 0;
    int requestsOverhead = 0;

protected:
    virtual void onWSM(WaveShortMessage* wsm);
    virtual void onWSA(WaveServiceAdvertisment* wsa);
    virtual void handleSelfMsg(cMessage* msg);
    virtual void handlePositionUpdate(cObject* obj);

    // server
    virtual void handleRequest(WaveShortMessage* wsm);
    virtual void handleNotification(WaveShortMessage* wsm);
    virtual void notifyRequest(std::string uuid, double estimatedDuration,
            double distance);
    virtual void notifyResponse();
    virtual void calculateResponse();
    virtual void sendResponse();

    //common
    virtual void sendWSM(FogMessage message, int psid = 0);
    virtual void finish();

    //client
    virtual void generateRequest(int type, double requirement);
    virtual void handleResponse(WaveShortMessage* wsm);

};

#endif
