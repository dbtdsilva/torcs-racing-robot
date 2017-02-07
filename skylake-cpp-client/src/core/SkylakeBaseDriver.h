#ifndef RAZERNAGA_SKYLAKEGEAR_H
#define RAZERNAGA_SKYLAKEGEAR_H

#include <core-common/CarState.h>
#include <core-common/WrapperBaseDriver.h>
#include <vector>

class SkylakeBaseDriver : public WrapperBaseDriver {
public:
    SkylakeBaseDriver();

    virtual CarControl wDrive(CarState cs) = 0;
    virtual void init(float *angles);
    virtual void onShutdown() { cout << "Bye bye!" << endl; };
    virtual void onRestart() { cout << "Restarting the race!" << endl; };

protected:
    virtual int getGear(CarState& cs, bool backwards = false);
    virtual float getRaceClutch(CarState& cs);

    vector<float> lrf_angles_;
    float clutch = 0;
};


#endif //RAZERNAGA_SKYLAKEGEAR_H
