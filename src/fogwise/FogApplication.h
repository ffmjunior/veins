#pragma once

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 * @author ffmjunior@hotmail.com
 */
class FogApplication {

public:
    FogApplication(double applicationSize);
    FogApplication(double applicationSize, int applicationID);

private:
    double applicationSize = 0;
    int applicationID = 0;

public:
    virtual double getApplicationSize();
    virtual void setApplicationSize(double applicationSize);
    virtual int getApplicationID();

};

