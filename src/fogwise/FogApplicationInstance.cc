#include "FogApplicationInstance.h"

FogApplicationInstance::FogApplicationInstance()
    {
    id = 0;
    uuid = "AA";
    load = 0;
    initialTimestamp = 0;
    completionTime = 0;
    applicationType = 0;
    codeSize = 0;
    }

FogApplicationInstance::FogApplicationInstance(double time) :
FogApplicationInstance(time, 20) {
}

FogApplicationInstance::FogApplicationInstance(double time, double load) {
    this->initialTimestamp = time;
    this->load = this->calculateLoad(load, load * 0.5); // Changes load
}

FogApplicationInstance::FogApplicationInstance(double time, double load, int applicationInstanceId) :
        FogApplicationInstance(time, load) {
    this->id = applicationInstanceId;
}

FogApplicationInstance::FogApplicationInstance(double time, double load, std::string uuid) :
        FogApplicationInstance(time, load) {
    this->uuid = uuid;
}

int FogApplicationInstance::waitTime(int currentTime) {
    return currentTime - this->initialTimestamp;
}

void FogApplicationInstance::tick() {
    this->completionTime = completionTime - 1;
}

int FogApplicationInstance::getId() {
    return id;
}

std::string FogApplicationInstance::getUUID(){
    return this->uuid;
}

void FogApplicationInstance::setCompletionTime(double completion) {
    this->completionTime = completion;
    //        System.out.println("Duration " + this.duration + " Load " + this.load);
}

bool FogApplicationInstance::isFinished() {
    if (this->completionTime <= 0) {
        return true;
    } else {
        return false;
    }
}

double FogApplicationInstance::calculateLoad(double reference, double deviation) {
    //NormalDistribution *nd = new NormalDistribution(reference, deviation);
    std::default_random_engine generator;
    std::normal_distribution<double> distribution(reference, deviation);
    load = distribution(generator);
    while (load < 0) {
        load = distribution(generator);
    }
    return load;
}

double FogApplicationInstance::getInitialTimestamp() {
    return initialTimestamp;
}

double FogApplicationInstance::getLoad() {
    return load;
}

bool FogApplicationInstance::compareIDs(FogApplicationInstance *ic) {
    return this->getId() == ic->getId();
}

double FogApplicationInstance::getDuration(){
return this->completionTime;
}
std::string FogApplicationInstance::info(){
    std::stringstream ss;
    ss << this->uuid << ";" << this->initialTimestamp << ";" << this->completionTime << ";" << this->id << ";" << this->applicationType << ";" << this->codeSize;
    //std::cout << "APPLICATION INSTANCE " << ss.str() << std::endl;
    return ss.str();
    }


