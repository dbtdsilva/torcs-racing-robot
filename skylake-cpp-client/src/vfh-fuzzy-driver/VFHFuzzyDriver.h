#ifndef FUZZYRIVER_H_
#define FUZZYRIVER_H_

#include "core-common/WrapperBaseDriver.h"

#include <fl/Headers.h>
#include <core/SkylakeBaseDriver.h>

class VFHFuzzyDriver : public SkylakeBaseDriver
{
public:
    VFHFuzzyDriver();
    ~VFHFuzzyDriver();

    bool loadFCLfile(std::string fclFile);
    virtual CarControl wDrive(CarState cs);
    float getSteer(CarState& cs);
    float filterABS(CarState &cs, float brake);
    float filterTCL(CarState &cs, float accel);
private:
    CarControl control_;
    fl::Engine *flEngine;
};

#endif /*FUZZYRIVER_H_*/
