#include "FuzzyDriver.h"

#include <iomanip>
#include <iostream>

FuzzyDriver::FuzzyDriver() {
}

FuzzyDriver::~FuzzyDriver() {
    delete flEngine;
}

bool FuzzyDriver::loadFCLfile(std::string fclFile) {
    bool fileLoaded = false;

    try
    {
        flEngine = fl::FclImporter().fromFile(fclFile);
        fileLoaded = true;
    }
    catch (fl::Exception &e)
    {
        std::cerr << "FuzzyLite: " << e.getWhat() << std::endl;
    }

    return fileLoaded;
}

CarControl FuzzyDriver::wDrive(CarState cs)
{
    float accel  = 0; // [0,1]
    float brake  = 0; // [0,1]
    int   gear   = 0; // {-1,..,6}
    float steer  = 0; // [-1,1]
    float clutch = 0; // [0,1]
    int   meta   = 0; // {0,1}
    int   focus  = 0; // [-90,90]

    float trackPos = cs.getTrackPos();
    float angle = cs.getAngle();
    float track9 = cs.getTrack(9);
    float speedX = cs.getSpeedX();

    flEngine->setInputValue("trackPos", trackPos);
    flEngine->setInputValue("angle", angle);
    flEngine->setInputValue("track9", track9);
    flEngine->setInputValue("speedX", speedX);

    flEngine->process();

    steer = (float)flEngine->getOutputVariable("steer")->getOutputValue();
    accel = (float)flEngine->getOutputVariable("accel")->getOutputValue();
    brake = (float)flEngine->getOutputVariable("brake")->getOutputValue();
    gear = getGear(cs);

    std::cout << '\r' << std::fixed << std::setw(7) << std::setprecision(3) << std::setfill('0') <<
        "trackPos: " << trackPos << ", angle: " << angle << ", speedX: " << speedX << ", track9: " << track9 << ", steer: " << steer << ", accel: " << accel << 
        ", brake: " << brake << std::flush;

    CarControl cc(accel, brake, gear, steer, clutch, focus, meta);
    return cc;
}