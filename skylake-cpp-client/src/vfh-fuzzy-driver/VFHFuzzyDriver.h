#ifndef FUZZYRIVER_H_
#define FUZZYRIVER_H_

#include "core/WrapperBaseDriver.h"

#include <fl/Headers.h>

class FuzzyDriver : public WrapperBaseDriver
{
public:

    // Constructor
    FuzzyDriver(BaseDriver::tstage stage, std::string fclFile);

    // Destructor
    ~FuzzyDriver();

    bool loadFCLfile();
    virtual CarControl wDrive(CarState cs);

    virtual void onShutdown();
    virtual void onRestart();

    virtual void init(float *angles);

    static const int gearUp[6];
    static const int gearDown[6];

    int getGear(CarState& cs);
    float getSteer(CarState& cs);

private:
    CarControl control_;
    std::string fclFile;

    fl::Engine *flEngine;
    vector<float> lrf_angles_;
};

#endif /*FUZZYRIVER_H_*/
