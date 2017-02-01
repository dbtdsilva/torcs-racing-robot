#include "VFHFuzzyDriver.h"

#include <iomanip>
#include <iostream>
#include <core/SkylakeConsts.h>

FuzzyDriver::FuzzyDriver(BaseDriver::tstage, std::string fclFile)
{
    this->stage = stage;
    this->fclFile = fclFile;
    this->control_ = CarControl(0, 0, 0, 0, 0);
}

FuzzyDriver::~FuzzyDriver()
{
    delete flEngine;
}

bool FuzzyDriver::loadFCLfile()
{
    bool fileLoaded = false;

    try {
        flEngine = fl::FclImporter().fromFile(fclFile);
        fileLoaded = true;
    } catch (fl::Exception &e) {
        std::cerr << "FuzzyLite: " << e.getWhat() << std::endl;
    }

    return fileLoaded;
}

void FuzzyDriver::init(float *angles)
{
    // set angles as {-90,-75,-60,-45,-30,20,15,10,5,0,5,10,15,20,30,45,60,75,90}
    for (int i = 0; i < 5; i++) {
        angles[i] = (float)(-90 + i * 15);
        angles[18 - i] = (float)(90 - i * 15);
    }

    for (int i = 5; i < 9; i++) {
        angles[i] = (float)(-20 + (i - 5) * 5);
        angles[18 - i] = (float)(20 - (i - 5) * 5);
    }
    angles[9] = 0;

    for (int i = 0; i < 19; i++)
        lrf_angles_.push_back(angles[i]);
}

void FuzzyDriver::onShutdown()
{
    std::cout << std::endl << "Bye bye!" << std::endl;
}

void FuzzyDriver::onRestart()
{
    std::cout << std::endl << "Restarting the race!" << std::endl;
}

/* Gear Changing Constants*/
const int FuzzyDriver::gearUp[6] =
{
    9900,9900,9900,9900,9900,0
};
const int FuzzyDriver::gearDown[6] =
{
    0,2500,3000,3000,3500,3500
};

CarControl FuzzyDriver::wDrive(CarState cs)
{
    int   gear   = 0; // {-1,..,6}
    float steer  = 0; // [-1,1]
    float clutch = 0; // [0,1]
    int   meta   = 0; // {0,1}
    int   focus  = 0; // [-90,90]

    float trackPos = cs.getTrackPos();
    float angle = cs.getAngle();
    float track9 = cs.getTrack(9);
    float speedX = cs.getSpeedX();


    float accel = control_.getAccel(), brake = control_.getBrake();
    if (cs.getSpeedX() < 150) {
        accel += 0.1;
    } else {
        accel -= 0.1;
    }
    accel = accel < 0.0 ? 0 : accel;
    accel = accel > 1 ? 1 : accel;

    /*flEngine->setInputValue("trackPos", trackPos);
    flEngine->setInputValue("angle", angle);
    flEngine->setInputValue("track9", track9);
    flEngine->setInputValue("speedX", speedX);

    flEngine->process();

    steer = (float)flEngine->getOutputVariable("steer")->getOutputValue();
    accel = (float)flEngine->getOutputVariable("accel")->getOutputValue();
    brake = (float)flEngine->getOutputVariable("brake")->getOutputValue();*/
    gear = getGear(cs);
    //steer = getSteer(cs);

    std::cout << '\r' << std::fixed << std::setw(7) << std::setprecision(3) << std::setfill('0')
              << "trackPos: " << trackPos << ", angle: " << angle << ", speedX: " << speedX << ", track9: "
              << track9 << ", steer: " << steer << ", accel: " << accel << ", brake: " << brake << std::flush;

    control_.setAccel(accel);
    control_.setBrake(brake);
    control_.setGear(gear);
    control_.setSteer(steer);
    control_.setClutch(clutch);
    control_.setFocus(focus);
    control_.setMeta(meta);
    return control_;
}

int FuzzyDriver::getGear(CarState &cs)
{
    int gear = cs.getGear();
    int rpm = cs.getRpm();

    // if gear is 0 (N) or -1 (R) just return 1 
    if (gear < 1)
        return 1;
    // check if the RPM value of car is greater than the one suggested 
    // to shift up the gear from the current one     
    if (gear < 6 && rpm >= gearUp[gear - 1])
        return gear + 1;
    else
        // check if the RPM value of car is lower than the one suggested 
        // to shift down the gear from the current one
        if (gear > 1 && rpm <= gearDown[gear - 1])
            return gear - 1;
        else // otherwhise keep current gear
            return gear;
}

float FuzzyDriver::getSteer(CarState &cs) {
    int max_id = -1;
    for (int i = 0; i < 19; i++) {
        if (max_id == -1 || cs.getTrack(max_id) <= cs.getTrack(i))
            max_id = i;
    }
    // steering angle is compute by correcting the actual car angle w.r.t. to track
    // axis [cs.getAngle()] and to adjust car position w.r.t to middle of track [cs.getTrackPos()*0.5]
    double targetAngle = (cs.getAngle() - ((lrf_angles_[max_id] * M_PI) / 180.0) );// * 0.5);
    // at high speed reduce the steering command to avoid loosing the control
    //if (cs.getSpeedX() > steerSensitivityOffset)
    //    return targetAngle / (steerLock * (cs.getSpeedX() - steerSensitivityOffset) * wheelSensitivityCoeff);
    //else
    return targetAngle / SkylakeConsts::STEER_LOCK_RAD;
}