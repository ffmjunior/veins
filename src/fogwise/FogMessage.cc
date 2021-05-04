#include "FogMessage.h"

FogMessage::FogMessage() :
        FogMessage(0.0, 0.0)
{
}

FogMessage::FogMessage(double distance, double estimatedProcessing) :
        FogMessage(0.0, 0.0, 0.0, Coord::ZERO)
{
    this->distance = distance;
    this->estimatedProcessing = estimatedProcessing;
}

FogMessage::FogMessage(double requirement, double dataSize, double load) :
        FogMessage(0.0, 0.0)
{
    this->dataSize = dataSize;
    this->requirement = requirement;
    this->load = load;

}

FogMessage::FogMessage(double requirement, double dataSize, double load, Coord coordinates) :
        FogMessage(0.0, 0.0, 0.0, Coord::ZERO, "fogwise")
{
    this->uuid = FogStatistics::generate_uuid_v4();
    this->randomBackoff = FogStatistics::random(INT_MAX);
    this->dataSize = dataSize;
    this->requirement = requirement;
    this->load = load;
    this->coordinate = coordinates;
    this->data = "fogwise";
}

FogMessage::FogMessage(double requirement, double dataSize, double load, Coord coordinates, std::string data)
{
    this->uuid = FogStatistics::generate_uuid_v4();
    this->dataSize = dataSize;
    this->requirement = requirement;
    this->load = load;
    this->coordinate = coordinates;
    this->data = data;
    this->timestamp = 0.0;
    this->rtt = 0.0;
}

FogMessage::FogMessage(std::string formattedRequest)
{
    std::vector<std::string> words;
    FogStatistics::split(formattedRequest, words);
    this->uuid = words[0];
    this->requirement = std::stod(words[1]);
    this->load = std::stod(words[2]);
    this->coordinate = FogStatistics::coordFromString(words[3]);
    this->data = words[4];
    this->distance = std::stod(words[5]);
    this->estimatedProcessing = std::stod(words[6]);
    this->randomBackoff = std::stol(words[7]);
    this->timestamp = std::stod(words[8]);
    this->rtt = std::stod(words[9]);
    this->applicationType = std::stol(words[10]);
}

void FogMessage::setRtt(double rtt)
{
    this->rtt = rtt;
}

std::string FogMessage::info()
{
    std::stringstream ss;
    ss << this->uuid << ";" << this->requirement << ";" << this->load << ";" << coordinate.messageReady() << ";"
            << this->data << ";" << this->distance << ";" << this->estimatedProcessing << ";" << this->randomBackoff
            << ";" << this->timestamp << ";" << this->rtt << ";" << this->applicationType;
    //std::cout << "FOGMESSAGE " << ss.str() << std::endl;
    return ss.str();
}

void FogMessage::setCoordinate(Coord c)
{
    this->coordinate = c;
}

void FogMessage::setCoordinate(std::string coordinate)
{

}

void FogMessage::setDistance(double distance)
{
    this->distance = distance;
}

void FogMessage::setEstimatedProcessing(double estimatedProcessing)
{
    this->estimatedProcessing = estimatedProcessing;
}

void FogMessage::setTimestamp(double timestamp)
{
    this->timestamp = timestamp;
}

void FogMessage::setUUID(std::string uuid)
{
    this->uuid = uuid;
}

//receivers

//FogMessage::FogMessage(WaveShortMessage* wsm) :FogMessage(wsm->getWsmData()){}

std::string FogMessage::getUUID()
{
    return this->uuid;
}

double FogMessage::getRequirement()
{
    //return std::stod(fields[1]);
    return this->requirement;
}

double FogMessage::getLoad()
{
    //return std::stod(fields[2]);
    return this->load;
}

Coord FogMessage::getCoordinates()
{
    //return FogStatistics::coordFromString(fields[3]);
    return this->coordinate;
}

std::string FogMessage::getData()
{
    //return fields[4];
    return this->data;
}

//for notification
double FogMessage::getDistance()
{
    return this->distance;
}

double FogMessage::getEstimatedProcessing()
{
    return this->estimatedProcessing;
}

int FogMessage::getRandomBackoff()
{
    return this->randomBackoff;
}

double FogMessage::getTimestamp()
{
    return this->timestamp;
}

double FogMessage::getRTT(){
    return this->rtt;
}

void FogMessage::setApplicationType(int applicationType)
{
    this->applicationType = applicationType;
}

void FogMessage::setApplicationType(int applicationType, int applicationTypeDeviation)
{
    this->applicationType = (int) FogStatistics::randomNormal(applicationType, applicationTypeDeviation);
}

int FogMessage::getApplicationType()
{
    return this->applicationType;
}
