#ifndef RAZERNAGA_KINEMATICS_H
#define RAZERNAGA_KINEMATICS_H

#include <tuple>
#include "core-common/CarState.h"

#define USE_YAW

class Kinematics {
public:
    Kinematics();

    void update_position(CarState &state);
    std::tuple<double, double> get_position() const;
    double get_angle() const;
    friend std::ostream& operator<<(std::ostream& os, const Kinematics& position);
private:
private:
    std::tuple<double, double> position_;
    double angle_;
};


#endif //RAZERNAGA_KINEMATICS_H
