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

#include "veins/modules/application/traci/FogWiseCar.h"

Define_Module(FogWiseCar);

void FogWiseCar::initialize(int stage) {
    BaseWaveApplLayer::initialize(stage);

    if (stage == 0) {
        sentMessage = false;
        lastDroveAt = simTime();
        currentSubscribedServiceId = -1;
    }
    //std::cout << "CAR ID " << myId << std::endl;
    respondRequestEvt = new cMessage("respond request evt", 13);
    cancelEvent(respondRequestEvt);
    fogNode.setProcessingCapacity(processingCapacity);
    fogNode.createApplicationSet(applicationTypeMean, applicationTypeDeviation);
    // disable vector to change name
    requestSizes.disable();

}

void FogWiseCar::onWSA(WaveServiceAdvertisment* wsa) {
    //std::cout << "CAR RECEIVED WSA " << std::endl;
    if (currentSubscribedServiceId == -1) {
        mac->changeServiceChannel(wsa->getTargetChannel());
        currentSubscribedServiceId = wsa->getPsid();
        if (currentOfferedServiceId != wsa->getPsid()) {
            stopService();
            startService((Channels::ChannelNumber) wsa->getTargetChannel(),
                    wsa->getPsid(), "Mirrored Traffic Service");
        }
    }
}

void FogWiseCar::onWSM(WaveShortMessage* wsm) {
    switch (wsm->getPsid()) {
    case 101:
        //statistics
        requestsReceived = requestsReceived + 1;
        if (!requestSizes.isEnabled()) {
            requestSizes.setName("requestSize");
            requestSizes.enable();
        }
        requestSizes.recordWithTimestamp(simTime(), wsm->getByteLength());
        // add application to queue and notifies all other vehicles
        std::cout << "CAR request " << myId << " - "
                << (simTime() - wsm->getTimestamp()) << " DATA: "
                << wsm->getWsmData() << std::endl;
        handleRequest(wsm->dup());
        break;
    case 102:
        // receives notifications and decides to keep or withdraw the request
        std::cout << "CAR notification " << myId << " - "
                << (simTime() - wsm->getTimestamp()) << " DATA: "
                << wsm->getWsmData() << std::endl;
        handleNotification(wsm->dup());
        break;
    case 103:
        //Ignore or discard current or scheduled jobs
        std::cout << "CAR response " << myId << " - "
                << (simTime() - wsm->getTimestamp()) << " DATA: "
                << wsm->getWsmData() << std::endl;
        break;
    }
}

void FogWiseCar::handleSelfMsg(cMessage* msg) {
    //scheduleAt(simTime() + 1, wsm);
    if (msg->getKind() == 13) {
        std::cout << "CAR " << myId << " ANSWERING AT " << simTime()
                << std::endl;
        this->sendResponse();
    }
}

void FogWiseCar::handlePositionUpdate(cObject* obj) {
}

//new

void FogWiseCar::handleRequest(WaveShortMessage* wsm) {
    //WaveShortMessage* message = wsm->dup();
    FogMessage fogMessage(wsm->getWsmData());
    if (fogNode.checkApplicationType(fogMessage.getApplicationType())) {
        std::cout << "REQUEST ACCEPTED " << std::endl;
        requestsAccepted = requestsAccepted + 1;
        simtime_t requestTime = simTime();
        double estimatedDuration = fogNode.newTask(fogMessage,
                requestTime.dbl());
        if (!respondRequestEvt->isScheduled()) {
            cancelEvent(respondRequestEvt);
            scheduleAt(simTime() + estimatedDuration, respondRequestEvt);
        }
    } else {
        std::cout << "REQUEST NOT ACCEPTED " << std::endl;
    }
    // send notification here
    // std::cout << "SIMTIME: " << responseTime << " RECEIVED AT " << requestTime.dbl() << " ESTIMATED " << estimatedDuration << " ESTIMATED COMPLETION " << (responseTime + estimatedDuration) << std::endl;
    // this->notifyRequest(fogMessage.getUUID(), estimatedDuration, mobility->getCurrentPosition().distance(fogMessage.getCoordinates()));
    delete (wsm);
}

void FogWiseCar::handleNotification(WaveShortMessage* wsm) {
    //WaveShortMessage* message = wsm->dup();
    FogMessage notification(wsm->getWsmData());
    double estimate = fogNode.checkNotification(notification);
    if (estimate == 0.0) {
        cancelEvent(respondRequestEvt);
    }
}

/**
 * Cria uma mensagem do tipo 102
 */
void FogWiseCar::notifyRequest(std::string uuid, double estimatedDuration,
        double distance) {

    int psid = 102;
    FogMessage notification;
    notification.setUUID(uuid);
    notification.setEstimatedProcessing(estimatedDuration);
    notification.setDistance(distance);
    //std::cout << "NOTIFICATION " << notification.info() << std::endl;
    this->sendWSM(notification, 102);
}

void FogWiseCar::notifyResponse() {
    FogMessage response = fogNode.getResponse();
    this->sendWSM(response, 103);
}

void FogWiseCar::calculateResponse() {

}

void FogWiseCar::sendResponse() {
    //metrics
    responsesSent = responsesSent + 1;
    FogMessage response = fogNode.getResponse();
    this->sendWSM(response, 103);
    // nextSchedule
    cancelEvent(respondRequestEvt);
    double currentJob = fogNode.getCurrentTaskDuration();
    std::cout << "RESPONDED. NEXT JOB " << currentJob << std::endl;
    if (currentJob != 0) {
        scheduleAt(simTime() + currentJob, respondRequestEvt);
    }
}

FogWiseCar::~FogWiseCar() {
    cancelAndDelete(respondRequestEvt);
}

void FogWiseCar::sendWSM(FogMessage message, int psid) {
    WaveShortMessage* wsm = new WaveShortMessage();
    populateWSM(wsm);
    wsm->setPsid(psid);
    wsm->setWsmData(message.info().c_str());
    wsm->setWsmLength(10);
    wsm->setByteLength(100);
    if (dataOnSch) {
        startService(Channels::SCH2, psid, "FogWise");
        //started service and server advertising, schedule message to self to send later
        scheduleAt(computeAsynchronousSendingTime(0.1, type_SCH), wsm);
    } else {
        //send right away on CCH, because channel switching is disabled
        sendDown(wsm);
    }

}

void FogWiseCar::finish() {
    BaseWaveApplLayer::finish();
    recordScalar("queueSize", fogNode.getQueueSize());
    recordScalar("queueDuration", fogNode.getQueueDuration());
    recordScalar("requestsReceived", requestsReceived);
    recordScalar("responsesSent", responsesSent);
    recordScalar("requestsAccepted", requestsAccepted);
}

