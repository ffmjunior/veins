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

#include "veins/modules/application/traci/FogWiseCarClient.h"

Define_Module(FogWiseCarClient);

int FogWiseCarClient::count = 0;

void FogWiseCarClient::initialize(int stage) {
    BaseWaveApplLayer::initialize(stage);
    FogWiseCarClient::count = FogWiseCarClient::count + 1;

    if (FogWiseCarClient::count % this->clientRatio == 0) {
        this->isClient = true;
    } else {
        this->isClient = false;
    }

    if (stage == 0) {
        sentMessage = false;
        lastDroveAt = simTime();
        currentSubscribedServiceId = -1;
    }
    std::cout << "CAR ID " << myId << " COUNT " << FogWiseCarClient::count
            << std::endl;
    if (isClient) {
        std::cout << "CLIENT " << std::endl;
    } else {
        std::cout << "SERVER " << std::endl;
        respondRequestEvt = new cMessage("respond request evt", 13);
        cancelEvent(respondRequestEvt);
        fogNode.setProcessingCapacity(processingCapacity);
        fogNode.createApplicationSet(applicationTypeMean,
                applicationTypeDeviation);
        // disable vector to change name
        requestSizes.disable();
    }

}

void FogWiseCarClient::onWSA(WaveServiceAdvertisment *wsa) {
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

void FogWiseCarClient::onWSM(WaveShortMessage *wsm) {
    if (isClient) {
        switch (wsm->getPsid()) {
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
            std::cout << "CLIENT response " << myId << " - " << simTime().dbl()
                    << " - " << wsm->getTimestamp() << " DATA: "
                    << wsm->getWsmData() << std::endl;
            this->handleResponse(wsm->dup());
            break;
        }

    } else {
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
}

void FogWiseCarClient::handleSelfMsg(cMessage *msg) {
    switch (msg->getKind()) { //client
    case SEND_BEACON_EVT: {
        //std::cout << "Self Message " << simTime().dbl() << std::endl;
        //crie e envia a requisição
        //std::cout << "RSU POSITION " << mobility->getCurrentPosition().info() << std::endl;
        generateRequest(10, 50);
        scheduleAt(simTime() + beaconInterval, sendBeaconEvt);
        break;
    }
    case 13: //server
        std::cout << "CAR " << myId << " ANSWERING AT " << simTime()
                << std::endl;
        this->sendResponse();
        break;
    }
}

void FogWiseCarClient::handlePositionUpdate(cObject *obj) {
}

//new

void FogWiseCarClient::handleRequest(WaveShortMessage *wsm) {
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
    //delete (wsm);
}

void FogWiseCarClient::handleNotification(WaveShortMessage *wsm) {
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
void FogWiseCarClient::notifyRequest(std::string uuid, double estimatedDuration,
        double distance) {

    int psid = 102;
    FogMessage notification;
    notification.setUUID(uuid);
    notification.setEstimatedProcessing(estimatedDuration);
    notification.setDistance(distance);
    //std::cout << "NOTIFICATION " << notification.info() << std::endl;
    this->sendWSM(notification, 102);
}

void FogWiseCarClient::notifyResponse() {
    FogMessage response = fogNode.getResponse();
    this->sendWSM(response, 103);
}

void FogWiseCarClient::calculateResponse() {

}

void FogWiseCarClient::sendResponse() {
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

void FogWiseCarClient::sendWSM(FogMessage message, int psid) {
    WaveShortMessage *wsm = new WaveShortMessage();
    populateWSM(wsm);
    wsm->setPsid(psid);
    wsm->setWsmData(message.info().c_str());
    if (dataOnSch) {
        startService(Channels::SCH2, psid, "FogWise");
        //started service and server advertising, schedule message to self to send later
        scheduleAt(computeAsynchronousSendingTime(0.1, type_SCH), wsm);
    } else {
        //send right away on CCH, because channel switching is disabled
        sendDown(wsm);
    }

}

void FogWiseCarClient::generateRequest(int type, double requirement) {
    WaveShortMessage *wsm = new WaveShortMessage();
    int appCode = (int) FogStatistics::randomNormal();
    double load = FogStatistics::randomNormal(this->applicationLoadMean,
            this->applicationLoadDeviation);
    FogMessage request(50.0, 1.0, load);
    request.setCoordinate(this->mobility->getCurrentPosition());
    request.setTimestamp(simTime().dbl());
    request.setApplicationType(applicationTypeMean, applicationTypeDeviation);
    this->requestQueue[request.getUUID()] = request;
    this->sendWSM(request, 101);
    this->requestsGenerated = requestsGenerated + 1;
    //delete (wsm); // don't know why, but this helps the object management
}

/**
 * Quando qualquer resposta chega, a RSU deve realizar os devidos processamentos:
 * - adicionar á fila de requisições geradas
 * - monitorar o desempenho das requisições recebidas
 */
void FogWiseCarClient::handleResponse(WaveShortMessage *wsm) {
    simtime_t requestTime = simTime();
    //WaveShortMessage* message = wsm->dup();
    FogMessage response(wsm->getWsmData());
    FogMessage request = this->requestQueue[response.getUUID()];
    //std::cout << "RESPONSE " << request.info() << std::endl;
    this->requestsOverhead = requestsOverhead + 1;
    if (request.getRTT() == 0) { // prevents duplications
        double rtt = requestTime.dbl() - request.getTimestamp();
        request.setRtt(requestTime.dbl() - request.getTimestamp());
        this->requestQueue[request.getUUID()] = request;
        //std::cout << "RESPONSE " << request.info() << std::endl;
        this->averageRtt = (this->averageRtt + rtt) / 2;
        this->requestsMet = requestsMet + 1;
    }
    //cancelAndDelete(wsm);
}

void FogWiseCarClient::finish() {
    BaseWaveApplLayer::finish();
    recordScalar("queueSize", fogNode.getQueueSize());
    recordScalar("queueDuration", fogNode.getQueueDuration());
    recordScalar("requestsReceived", requestsReceived);
    recordScalar("responsesSent", responsesSent);
    recordScalar("requestsAccepted", requestsAccepted);

    //client
    recordScalar("averageRtt", averageRtt);
    recordScalar("requestsGenerated", requestsGenerated);
    recordScalar("requestsMet", requestsMet);
    recordScalar("requestsOverhead", (requestsOverhead - requestsMet));
    std::cout << "SUCCESSFUL FINISH  " << myId << std::endl;
}

FogWiseCarClient::~FogWiseCarClient() {
    //cancelAndDelete(respondRequestEvt);
    std::cout << "SUCCESSFUL DESTRUCTCOR  " << myId << std::endl;
}
