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

#include "veins/modules/application/traci/FogWiseRSUMonitor.h"

Define_Module(FogWiseRSUMonitor);

void FogWiseRSUMonitor::initialize(int stage) {
    BaseWaveApplLayer::initialize(stage);
}

void FogWiseRSUMonitor::onWSA(WaveServiceAdvertisment *wsa) {

}

void FogWiseRSUMonitor::onWSM(WaveShortMessage *wsm) {

}

void FogWiseRSUMonitor::handleSelfMsg(cMessage *msg) {
  std::cout << "SIMULATION RUNNING " << simTime() << std::endl;
  scheduleAt (simTime () + beaconInterval, sendBeaconEvt);

}

/**
 * Gera uma requisi��o.
 * Tenho que decidir como esta fun��o gerar� os valores dos tipos ou dos requisitos.
 * Tenho que fazer isso de forma que seja poss�vel configurar atrav�s do omnetpp.ini
 * para que eu possa realizar experimentos.
 * Por enquanto, vou colocar os geradores ou os vetores dentro da classe ou mesmo
 * numa classe acess�ria.
 * Al�m disso, esta fun��o deve:
 * - adicionar requisi��es criadas a uma fila
 * - monitorar o desempenho das requisi��es (expiradas)
 */
void FogWiseRSUMonitor::generateRequest(int type, double requirement) {

}

/**
 * Quando qualquer resposta chega, a RSU deve realizar os devidos processamentos:
 * - adicionar � fila de requisi��es geradas
 * - monitorar o desempenho das requisi��es recebidas
 */
void FogWiseRSUMonitor::handleResponse(WaveShortMessage *wsm) {

}

void FogWiseRSUMonitor::sendWSM(FogMessage message, int psid) {

}

void FogWiseRSUMonitor::finish() {

}
