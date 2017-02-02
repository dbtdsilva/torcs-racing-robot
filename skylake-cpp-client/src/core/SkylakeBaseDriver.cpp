#include "SkylakeBaseDriver.h"
#include "SkylakeConsts.h"

SkylakeBaseDriver::SkylakeBaseDriver() {

}

int SkylakeBaseDriver::getGear(CarState &cs, bool backwards) {
    int gear = cs.getGear();
    int rpm = cs.getRpm();

    if (backwards)
        return -1;

    if (gear == 0 || gear < -1)
        return 1;

    if (gear < 6 && rpm >= SkylakeConsts::GEAR_UP[gear - 1])
        return gear + 1;

    if (gear > 1 && rpm <= SkylakeConsts::GEAR_DOWN[gear - 1])
        return gear - 1;
    return gear;
}

float SkylakeBaseDriver::getRaceClutch(CarState &cs) {
    static float clutch = 0;
    float max_clutch = SkylakeConsts::CLUTCH_MAX;

    // Check if the current situation is the race start
    if (cs.getCurLapTime() < SkylakeConsts::CLUTCH_DELTA_TIME && cs.getDistRaced() < SkylakeConsts::CLUTCH_DELTA_RACED)
        clutch = max_clutch;

    // Adjust the current value of the clutch
    if (clutch > 0) {
        double delta = SkylakeConsts::CLUTCH_DELTA;
        if (cs.getSpeedX() <= 10) {
            // Apply a stronger clutch output when the gear is one and the race is just started
            delta /= 2;
            max_clutch *= SkylakeConsts::CLUTCH_MAX_MODIFIER;
            if (cs.getCurLapTime() < SkylakeConsts::CLUTCH_MAX_TIME)
                clutch = max_clutch;
        }

        // check clutch is not bigger than maximum values
        clutch = min(max_clutch, clutch);

        // if clutch is not at max value decrease it quite quickly
        if (clutch != max_clutch) {
            clutch -= delta;
            clutch = static_cast<float>(max(0.0, double(clutch)));
        } // if clutch is at max value decrease it very slowly
        else {
            clutch -= SkylakeConsts::CLUTCH_DEC;
        }
    }
    return clutch;
}

void SkylakeBaseDriver::init(float *angles) {
    // set angles as {-90, -75, -60, -45, -30, -20, -15, -10, -5, 0, 5, 10, 15, 20, 30, 45, 60, 75, 90}
    for (int i = 0; i < 5; i++) {
        angles[i] = -90 + i * 15;
        angles[18 - i] = 90 - i * 15;
    }

    for (int i = 5; i < 9; i++) {
        angles[i] = -20 + (i - 5) * 5;
        angles[18 - i] = 20 - (i - 5) * 5;
    }
    angles[9] = 0;

    for (int i = 0; i < 19; i++) {
        lrf_angles_.push_back(angles[i]);
    }
}
