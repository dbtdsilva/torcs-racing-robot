#ifndef SIMPLEDRIVER_H_
#define SIMPLEDRIVER_H_

#include <iostream>
#include <cmath>
#include "core/BaseDriver.h"
#include "core/CarState.h"
#include "core/CarControl.h"
#include "core/SimpleParser.h"
#include "core/WrapperBaseDriver.h"
#include "Kinematics.h"
#include "Map.h"

using namespace std;

class SkylakeDriver : public WrapperBaseDriver {
public:
    SkylakeDriver();
    virtual CarControl wDrive(CarState cs);
    virtual void onShutdown();
    virtual void onRestart();
    virtual void init(float *angles);

private:
    /* Gear Changing Constants*/
    // RPM values to change gear
    static const int gearUp[6];
    static const int gearDown[6];

    /* Stuck constants*/
    // How many time steps the controller wait before recovering from a stuck position
    static const int stuckTime;
    // When car angle w.r.t. track axis is grather tan stuckAngle, the car is probably stuck
    static const float stuckAngle;

    /* Steering constants*/
    // Angle associated to a full steer command
    static const float steerLock;
    // Min speed to reduce steering command
    static const float steerSensitivityOffset;
    // Coefficient to reduce steering command at high speed (to avoid loosing the control)
    static const float wheelSensitivityCoeff;

    /* Accel and Brake Constants*/
    // max speed allowed
    static const float maxSpeed;
    // Min distance from track border to drive at  max speed
    static const float maxSpeedDist;
    // pre-computed sin5
    static const float sin5;
    // pre-computed cos5
    static const float cos5;

    /* ABS Filter Constants */
    // Radius of the 4 wheels of the car
    static const float wheelRadius[4];
    // min slip to prevent ABS
    static const float absSlip;
    // range to normalize the ABS effect on the brake
    static const float absRange;
    // min speed to activate ABS
    static const float absMinSpeed;

    /* Clutch constants */
    static const float clutchMax;
    static const float clutchDelta;
    static const float clutchRange;
    static const float clutchDeltaTime;
    static const float clutchDeltaRaced;
    static const float clutchDec;
    static const float clutchMaxModifier;
    static const float clutchMaxTime;

    int stuck;
    float clutch;

    int getGear(CarState &cs);
    float getSteer(CarState &cs);
    float getAccel(CarState &cs);
    float filterABS(CarState &cs, float brake);
    void clutching(CarState &cs, float &clutch);

    CarControl control;
    Map map_;
    Kinematics kinematics_;
};

#endif /*SIMPLEDRIVER_H_*/
