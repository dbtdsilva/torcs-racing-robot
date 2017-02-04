#include "VFHFuzzyDriver.h"

#include <iomanip>
#include <iostream>
#include <core/SkylakeConsts.h>

VFHFuzzyDriver::VFHFuzzyDriver()
{
    this->control_ = CarControl(0, 0, 0, 0, 0);
}

VFHFuzzyDriver::~VFHFuzzyDriver()
{
    delete flEngine;
}

bool VFHFuzzyDriver::loadFCLfile(std::string fclFile)
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

CarControl VFHFuzzyDriver::wDrive(CarState cs)
{
    int gear = 0; // {-1,.., 6}
    float steer = 0; // [-1, 1]
    float clutch = 0; // [0, 1]
    int meta = 0; // {0, 1}
    int focus = 0; // [-90, 90]
    float accel = 0; // [0, 1]
    float brake = 0; // [0, 1]

    float trackPos = cs.getTrackPos(); // [-1, 1]
    float angle = cs.getAngle(); // [-3.15, 3.15]
    //float track9 = cs.getTrack(9); // [0, 200.0]
    float speedX = cs.getSpeedX(); // [0, inf]

    int max_id = -1;
    for (int i = 0; i < 19; i++) {
        if (max_id == -1 || cs.getTrack(max_id) <= cs.getTrack(i))
            max_id = i;
    }
    double max_angle = -(lrf_angles_[max_id] * M_PI) / 180.0; // [-3.15, 3.15]
    float dist_max_angle = cs.getTrack(max_id);

    double diff_angle = abs(max_angle - angle);

    flEngine->setInputValue("angle", angle);
    flEngine->setInputValue("maxAngle", max_angle);
    flEngine->setInputValue("diffAngle", diff_angle);
    flEngine->setInputValue("distFront", cs.getTrack(9));
    flEngine->setInputValue("distMaxAngle", dist_max_angle);
    flEngine->setInputValue("trackPos", trackPos);
    flEngine->setInputValue("speedX", speedX);

    flEngine->process();

    steer = (float)flEngine->getOutputVariable("steer")->getOutputValue();
    accel = (float)flEngine->getOutputVariable("accel")->getOutputValue();
    brake = (float)flEngine->getOutputVariable("brake")->getOutputValue();
    gear = getGear(cs);
    //steer = getSteer(cs);

    std::cout << std::fixed << std::setw(7) << std::setprecision(5) << std::setfill('0')
              << "trackPos: " << trackPos << ", speedX: " << speedX << ", angle: " << angle << ", maxAngle: "
              << max_angle << ", steer: " << steer << ", accel: " << accel << ", brake: " << brake
              << ", distFront: " << cs.getTrack(9) << ", distMaxAngle: " << dist_max_angle << std::endl;
    control_.setAccel(accel);
    control_.setBrake(brake);
    control_.setGear(gear);
    control_.setSteer(steer);
    control_.setClutch(clutch);
    control_.setFocus(focus);
    control_.setMeta(meta);
    return control_;
}

float VFHFuzzyDriver::getSteer(CarState &cs) {

    // steering angle is compute by correcting the actual car angle w.r.t. to track
    // axis [cs.getAngle()] and to adjust car position w.r.t to middle of track [cs.getTrackPos()*0.5]
    //double targetAngle = (cs.getAngle() - ((lrf_angles_[max_id] * M_PI) / 180.0) );// * 0.5);
    double targetAngle = (cs.getAngle() - 0.5 * cs.getTrackPos());
    // at high speed reduce the steering command to avoid loosing the control
    //if (cs.getSpeedX() > steerSensitivityOffset)
    //    return targetAngle / (steerLock * (cs.getSpeedX() - steerSensitivityOffset) * wheelSensitivityCoeff);
    //else
    return targetAngle / SkylakeConsts::STEER_LOCK_RAD;
}