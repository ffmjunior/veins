//
// Copyright (C) 2016 David Eckhoff <david.eckhoff@fau.de>
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

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "veins/modules/application/traci/FogWiseRSU.h"

Define_Module(FogWiseRSU);

void FogWiseRSU::initialize(int stage)
{
    BaseWaveApplLayer::initialize(stage);
    //std::cout << "RSU Stage " << stage << " - " << simTime().dbl() << std::endl;
    BaseMobility *baseMob;
    baseMob = FindModule<BaseMobility*>::findSubModule(getParentModule());
    rsuCoord = baseMob->getCurrentPosition();
    //std::cout << "RSU POSITION " << rsuCoord.info() << std::endl;
}

void FogWiseRSU::onWSA(WaveServiceAdvertisment* wsa)
{
    //if this RSU receives a WSA for service 42, it will tune to the chan
    if (wsa->getPsid() == 42) {
        mac->changeServiceChannel(wsa->getTargetChannel());
    }
}

void FogWiseRSU::onWSM(WaveShortMessage* wsm)
{
    // posso ter que usar isso
    //sendDelayedDown(wsm->dup(), 2 + uniform(0.01, 0.2));
    switch (wsm->getPsid()) {
        case 101:
            // ignore
            //std::cout << "Request Received. Ignoring..." << std::endl;
            // handleRequest(wsm);
            delete (wsm);
            break;
        case 102:
            // ignore
            std::cout << "RSU Request Notification Received. Ignoring... " << wsm->getWsmData() << std::endl;
            // handleNotification(wsm);
            break;
        case 103:
            // fullfillment and statistics
            std::cout << "RSU response " << myId << " - " << simTime().dbl() << " - " << wsm->getTimestamp()
                    << " DATA: " << wsm->getWsmData() << std::endl;
            this->handleResponse(wsm->dup());
            break;
    }
}

void FogWiseRSU::handleSelfMsg(cMessage* msg)
{
    switch (msg->getKind()) {
        case SEND_BEACON_EVT: {
            //std::cout << "Self Message " << simTime().dbl() << std::endl;
            //crie e envia a requisição
            //std::cout << "RSU POSITION " << mobility->getCurrentPosition().info() << std::endl;
            generateRequest(10, 50);
            scheduleAt(simTime() + beaconInterval, sendBeaconEvt);
            break;
        }
    }
}

/**
 * Gera uma requisição.
 * Tenho que decidir como esta função gerará os valores dos tipos ou dos requisitos.
 * Tenho que fazer isso de forma que seja possível configurar através do omnetpp.ini
 * para que eu possa realizar experimentos.
 * Por enquanto, vou colocar os geradores ou os vetores dentro da classe ou mesmo
 * numa classe acessória.
 * Além disso, esta função deve:
 * - adicionar requisições criadas a uma fila
 * - monitorar o desempenho das requisições (expiradas)
 */
void FogWiseRSU::generateRequest(int type, double requirement)
{
    WaveShortMessage* wsm = new WaveShortMessage();
    int appCode = (int) FogStatistics::randomNormal();
    double load = FogStatistics::randomNormal(this->applicationLoadMean, this->applicationLoadDeviation);
    FogMessage request(50.0, 1.0, load);
    request.setCoordinate(this->rsuCoord);
    request.setTimestamp(simTime().dbl());
    request.setApplicationType(applicationTypeMean, applicationTypeDeviation);
    this->requestQueue[request.getUUID()] = request;
    this->sendWSM(request, 101);
    this->requestsGenerated = requestsGenerated + 1;
    delete (wsm); // don't know why, but this helps the object management
}

/**
 * Quando qualquer resposta chega, a RSU deve realizar os devidos processamentos:
 * - adicionar á fila de requisições geradas
 * - monitorar o desempenho das requisições recebidas
 */
void FogWiseRSU::handleResponse(WaveShortMessage* wsm)
{
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
    cancelAndDelete(wsm);
}

void FogWiseRSU::sendWSM(FogMessage message, int psid)
{
    WaveShortMessage* wsm = new WaveShortMessage();
    populateWSM(wsm);
    wsm->setPsid(psid);
    wsm->setWsmData(message.info().c_str());
    if (dataOnSch) {
        startService(Channels::SCH2, psid, "FogWise");
        //started service and server advertising, schedule message to self to send later
        scheduleAt(computeAsynchronousSendingTime(1, type_SCH), wsm);
    }
    else {
        //send right away on CCH, because channel switching is disabled
        sendDown(wsm);
    }
}

void FogWiseRSU::finish()
{
    BaseWaveApplLayer::finish();
    recordScalar("averageRtt", averageRtt);
    recordScalar("requestsGenerated", requestsGenerated);
    recordScalar("requestsMet", requestsMet);
    recordScalar("requestsOverhead", (requestsOverhead - requestsMet));
}
