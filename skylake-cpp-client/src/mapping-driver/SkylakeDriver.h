#ifndef SIMPLEDRIVER_H_
#define SIMPLEDRIVER_H_

#include <iostream>
#include <cmath>
#include <core/SkylakeBaseDriver.h>
#include "core-common/BaseDriver.h"
#include "core-common/CarState.h"
#include "core-common/CarControl.h"
#include "core-common/SimpleParser.h"
#include "core-common/WrapperBaseDriver.h"
#include "Kinematics.h"
#include "Map.h"

using namespace std;

class SkylakeDriver : public SkylakeBaseDriver {
public:
    SkylakeDriver();
    virtual CarControl wDrive(CarState cs);
private:
    float getSteer(CarState &cs);

    CarControl control;
    Map map_;
    Kinematics kinematics_;

};

#endif /*SIMPLEDRIVER_H_*/
