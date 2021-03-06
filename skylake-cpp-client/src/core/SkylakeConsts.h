#ifndef SKYLAKECONSTS_H
#define SKYLAKECONSTS_H

#include <cmath>

#define M_X(arg)        std::get<0>(arg)
#define M_Y(arg)        std::get<1>(arg)

class SkylakeConsts {
public:
    static constexpr double STEER_LOCK_DEGREES = 21.0;
    static constexpr double STEER_LOCK_RAD = (STEER_LOCK_DEGREES * M_PI) / 180.0;
    static constexpr double RIM_DIAM = 0.4572;

    static constexpr double TIRE_FRONT_WIDTH = 0.255;
    static constexpr double TIRE_FRONT_HEIGHT = TIRE_FRONT_WIDTH * 0.4;
    static constexpr double WHEEL_FRONT_RADIUS = TIRE_FRONT_HEIGHT + RIM_DIAM / 2.0;
    static constexpr double WHEELS_FRONT_DISTANCE = 1.68;

    static constexpr double TIRE_BACK_WIDTH = 0.330;
    static constexpr double TIRE_BACK_HEIGHT = TIRE_BACK_WIDTH * 0.3;
    static constexpr double WHEEL_BACK_RADIUS = TIRE_BACK_HEIGHT + RIM_DIAM / 2.0;


    static constexpr int MAP_SQUARE_SIZE = 4;
    static constexpr double MAP_SAFE_MARGIN = 0.2;

    static constexpr float CLUTCH_MAX = 0.55;
    static constexpr float CLUTCH_DELTA = 0.01;
    static constexpr float CLUTCH_RANGE = 1;
    static constexpr float CLUTCH_DELTA_TIME = 5;//0.02;
    static constexpr float CLUTCH_DELTA_RACED = 15;
    static constexpr float CLUTCH_DEC = 0.001;
    static constexpr float CLUTCH_MAX_MODIFIER = 1.25;
    static constexpr float CLUTCH_MAX_TIME = 7;//1.5;

    // Since we do not damage the engine, the gear up limit for each gear is the redline.
    // In this car, the top torque happens after the red line for every gear shift.
    static const int GEAR_UP[6];
    // Those gear down were defined based on the top torque, to keep a good torque after changing the gear.
    static const int GEAR_DOWN[6];
};
#endif //SKYLAKECONSTS_H
