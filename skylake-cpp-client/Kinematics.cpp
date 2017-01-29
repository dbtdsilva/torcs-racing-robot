#include "Kinematics.h"
#include "SkylakeConsts.h"

double convert_wheel_speed_to_distance(float speed_rad_s, double factor) {
    double speed_m_s = speed_rad_s * SkylakeConsts::WHEEL_FRONT_RADIUS;
    return factor * speed_m_s;
}

Kinematics::Kinematics() : angle_(0), position_(0.0, 0.0) {

}

double Kinematics::get_angle() const {
    return 0;
}

tuple<double, double> Kinematics::get_position() const {
    return position_;
}

void Kinematics::update_position(CarState &cs) {
    static double last_distance = 0;
    double distance = cs.getDistRaced() - last_distance;
    last_distance = cs.getDistRaced();

#ifndef USE_YAW
    double speed_norm = sqrt(pow(cs.getSpeedX(), 2) + pow(cs.getSpeedY(), 2) + pow(cs.getSpeedZ(), 2));
    double speed_norm_m_per_s = speed_norm / 3.6;

    double factor = distance / speed_norm_m_per_s;

    double distance_right_wheel = convert_wheel_speed_to_distance(cs.getWheelSpinVel(0), factor);
    double distance_left_wheel = convert_wheel_speed_to_distance(cs.getWheelSpinVel(1), factor);

    double distance_wheels = (distance_right_wheel + distance_left_wheel) / 2.0;
    M_X(position_) += distance_wheels * cos(angle_);
    M_Y(position_) += distance_wheels * sin(angle_);
    angle_ += (distance_right_wheel - distance_left_wheel) / SkylakeConsts::WHEELS_FRONT_DISTANCE;
#else
    M_X(position_) += distance * cos(angle_);
    M_Y(position_) += distance * sin(angle_);
    angle_ = cs.getYaw();
#endif
}

ostream& operator<<(ostream& os, const Kinematics& obj) {
    os << "P: " << get<0>(obj.position_) << ", " << get<1>(obj.position_) << ", A: " << (obj.angle_ * 180.0) / M_PI;
}
