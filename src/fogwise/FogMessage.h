#pragma once

/*
 * FogStatistics.h
 *
 * Sources
 * http://www.martinbroadhurst.com/how-to-split-a-string-in-c.html
 * https://stackoverflow.com/questions/24365331/how-can-i-generate-uuid-in-c-without-using-boost-library
 *
 *  Created on: 29 de abr de 2020
 *      Author: ffmju
 */

#ifndef SRC_FOGWISE_FOGMESSAGE_H_
#define SRC_FOGWISE_FOGMESSAGE_H_

#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include "veins/base/utils/Coord.h"
#include "veins/modules/messages/WaveShortMessage_m.h"


#include "FogStatistics.h"

class FogMessage {

private:
    //request
    std::string uuid;
    double requirement;
    double load;
    Coord coordinate;
    std::string data;
    int applicationType = 0;

    //notify
    double distance;
    double estimatedProcessing;
    uint64_t randomBackoff;

    //response
    double dataSize = 0.0;
    double timestamp = 0.0;
    double rtt = 0.0;

    // receivers
    std::vector<std::string> fields;

public:
    //for senders
    FogMessage();
    // for notification
    FogMessage(double distance, double estimatedProcessing);
    FogMessage(double requirement, double dataSize, double load);
    FogMessage(double requirement, double dataSize, double load,
            Coord coordinates);
    FogMessage(double requirement, double dataSize, double load,
            Coord coordinates, std::string data);

    // for reconstruction
    FogMessage(std::string formattedRequest);

    //for receivers
    //FogMessage(WaveShortMessage* wsm);

    //setters
    virtual void setApplicationType(int applicationType);
    virtual void setApplicationType(int applicationType, int applicationTypeDeviation);
    virtual void setCoordinate(Coord c);
    virtual void setCoordinate(std::string coordinate);
    virtual void setDistance(double distance);
    virtual void setEstimatedProcessing(double estimatedProcessing);
    virtual void setRtt(double rtt);
    virtual void setTimestamp(double timestamp);
    virtual void setUUID(std::string uuid);
    virtual std::string info();

    //for receivers
    virtual int getApplicationType();
    virtual Coord getCoordinates();
    virtual std::string getData();
    virtual double getLoad();
    virtual double getRequirement();
    virtual double getRTT();
    virtual double getTimestamp();
    virtual std::string getUUID();

    //for notification
    virtual double getDistance();
    virtual double getEstimatedProcessing();
    virtual int getRandomBackoff();

};

#endif /* SRC_FOGWISE_FOGMESSAGE_H */
