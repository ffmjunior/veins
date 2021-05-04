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

#ifndef FogWiseRSU11p_H
#define FogWiseRSU11p_H

#include <random>

#include "veins/modules/application/ieee80211p/BaseWaveApplLayer.h"


#include "fogwise/FogMessage.h"
#include "fogwise/FogStatistics.h"

/**
 * Small RSU Demo using 11p
 */
class FogWiseRSU : public BaseWaveApplLayer
{

private:
    std::map<std::string, FogMessage> requestQueue;
    //parameters
    double applicationLoadMean = 1100.0;
    double applicationLoadDeviation = 100.0;
    long applicationType;
    long applicationTypeMean = 100;
    long applicationTypeDeviation = 5;
    //metrics
    double averageRtt = 0.0;
    int requestsGenerated = 0;
    int requestsMet = 0;
    int requestsOverhead = 0;

public:
    virtual void initialize(int stage);

protected:
    Coord rsuCoord;
    virtual void onWSM(WaveShortMessage* wsm);
    virtual void onWSA(WaveServiceAdvertisment* wsa);
    virtual void handleSelfMsg(cMessage* msg);
    virtual void sendWSM(FogMessage message, int psid);

    //new
    virtual void generateRequest(int type, double requirement);
    virtual void handleResponse(WaveShortMessage* wsm);

    //override
    virtual void finish();

};

#endif
