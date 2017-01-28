#ifndef RAZERNAGA_SKYLAKECONSTS_H
#define RAZERNAGA_SKYLAKECONSTS_H

#include <cmath>

class SkylakeConsts {
public:
    static constexpr double STEER_LOCK_DEGREES = 21;
    static constexpr double STEER_LOCK_RAD = (STEER_LOCK_DEGREES * M_PI) / 180.0;
    static constexpr double TIRE_WIDTH = 0.255;
    static constexpr double TIRE_HEIGHT = TIRE_WIDTH * 0.4;
    static constexpr double RIM_DIAM = 0.4572;
    static constexpr double WHEEL_FRONT_RADIUS = TIRE_HEIGHT + RIM_DIAM / 2.0;
};
#endif //RAZERNAGA_SKYLAKECONSTS_H
