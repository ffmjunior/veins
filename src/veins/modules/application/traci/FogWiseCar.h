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

class FogWiseCar : public BaseWaveApplLayer
{
public:
    virtual void initialize(int stage);
    long int processingCapacity = 1000;
    long applicationTypeMean = 100;
    long applicationTypeDeviation = 5;
    ~FogWiseCar();
protected:
    simtime_t lastDroveAt;
    bool sentMessage;
    int currentSubscribedServiceId;
    FogNode fogNode;

    cMessage* respondRequestEvt;
    //metrics
    int queueSize = 0;
    double queueDuration = 0.0;
    int requestsReceived = 0;
    int responsesSent = 0;
    int requestsAccepted = 0;
    cOutVector requestSizes ;

protected:
    // basic methods
    virtual void onWSM(WaveShortMessage* wsm);
    virtual void onWSA(WaveServiceAdvertisment* wsa);
    virtual void handleSelfMsg(cMessage* msg);
    virtual void handlePositionUpdate(cObject* obj);

    // fogwise
    virtual void calculateResponse();
    virtual void handleNotification(WaveShortMessage* wsm);
    virtual void handleRequest(WaveShortMessage* wsm);
    virtual void notifyRequest(std::string uuid, double estimatedDuration, double distance);
    virtual void notifyResponse();
    virtual void sendResponse();
    virtual void sendWSM(FogMessage message, int psid = 0);

    // overload
    virtual void finish();

};

#endif
