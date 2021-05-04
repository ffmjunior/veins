#include "FogApplication.h"

FogApplication::FogApplication(double applicationSize) {
    this->applicationSize = applicationSize;
}

FogApplication::FogApplication(double applicationSize, int applicationID) {
    this->applicationSize = applicationSize;
    this->applicationID = applicationID;
}

double FogApplication::getApplicationSize() {
    return applicationSize;
}

void FogApplication::setApplicationSize(double applicationSize) {
    this->applicationSize = applicationSize;
}

int FogApplication::getApplicationID() {
    return applicationID;
}
