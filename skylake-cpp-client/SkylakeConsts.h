#ifndef RAZERNAGA_SKYLAKECONSTS_H
#define RAZERNAGA_SKYLAKECONSTS_H

#include <cmath>

#define M_X(arg)     std::get<0>(arg)
#define M_Y(arg)     std::get<1>(arg)

class SkylakeConsts {
public:
    static constexpr double STEER_LOCK_DEGREES = 21;
    static constexpr double STEER_LOCK_RAD = (STEER_LOCK_DEGREES * M_PI) / 180.0;
    static constexpr double TIRE_WIDTH = 0.255;
    static constexpr double TIRE_HEIGHT = TIRE_WIDTH * 0.4;
    static constexpr double RIM_DIAM = 0.4572;
    static constexpr double WHEEL_FRONT_RADIUS = TIRE_HEIGHT + RIM_DIAM / 2.0;
    static constexpr double WHEELS_FRONT_DISTANCE = 1.68;
};
#endif //RAZERNAGA_SKYLAKECONSTS_H
