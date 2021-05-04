#pragma once

#include <iostream>
#include <random>
#include <sstream>

/**
 * @author ffmjunior@hotmail.com
 */

class FogApplicationInstance {

protected:
    int id = 0;
    std::string uuid = "";
    double load = 0;
    double initialTimestamp = 0;
    double completionTime = 0;
    long applicationType = 0;
    int codeSize = 0; // future use

public:
    FogApplicationInstance();
    FogApplicationInstance(double time);
    FogApplicationInstance(double time, double load);
    FogApplicationInstance(double time, double load, int applicationInstanceId);
    FogApplicationInstance(double time, double load, std::string uuid);

    virtual int waitTime(int currentTime);
    virtual void tick();
    virtual int getId();
    virtual std::string getUUID();
    virtual void setCompletionTime(double completion);
    virtual bool isFinished();
    virtual double calculateLoad(double reference, double deviation);
    virtual double getInitialTimestamp();
    virtual double getLoad();
    virtual bool compareIDs(FogApplicationInstance *ic);
    virtual double getDuration();
    virtual std::string info();


};
