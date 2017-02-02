#ifndef FUZZYRIVER_H_
#define FUZZYRIVER_H_

#include "core-common/WrapperBaseDriver.h"

#include <fl/Headers.h>
#include <core/SkylakeBaseDriver.h>

class VFHFuzzyDriver : public WrapperBaseDriver
{
public:

    // Constructor
    VFHFuzzyDriver(BaseDriver::tstage stage, std::string fclFile);

    // Destructor
    ~VFHFuzzyDriver();

    bool loadFCLfile();
    virtual CarControl wDrive(CarState cs);

    virtual void onShutdown();
    virtual void onRestart();

    virtual void init(float *angles);

    static const int gearUp[6];
    static const int gearDown[6];

    float getSteer(CarState& cs);

private:
    CarControl control_;
    std::string fclFile;

    fl::Engine *flEngine;
    vector<float> lrf_angles_;
};

#endif /*FUZZYRIVER_H_*/
