#include "SkylakeDriver.h"
#include "core/SkylakeConsts.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

SkylakeDriver::SkylakeDriver() : control(0, 0, 0, 0, 0), kinematics_() {
    //map_.enable_debug();
    //map_.render_map();
}

CarControl SkylakeDriver::wDrive(CarState cs)
{
    static int speed = 25;
    if (cs.getSpeedX() >= speed) {
        cout << speed << " : " << cs.getDistRaced() << endl;
        speed += 25;
    }

    // compute acceleration/brake command
    //float accel_and_brake = getAccel(cs);
    // compute gear
    int gear = getGear(cs);
    // compute steering
    float steer = getSteer(cs);


    // normalize steering
    if (steer < -1)
        steer = -1;
    if (steer > 1)
        steer = 1;

    float accel = control.getAccel(), brake = control.getBrake();
    /*if (cs.getSpeedX() < 150) {
        accel += 0.1;
    } else {
        accel -= 0.1;
    }
    accel = accel < 0.0 ? 0 : accel;
    accel = accel > 1 ? 1 : accel;*/
    accel = 1;

    static double last_distance = 0;
    double distance = cs.getDistRaced() - last_distance;

    kinematics_.update_position(cs);
    map_.increase_visited_counter(M_X(kinematics_.get_position()), M_Y(kinematics_.get_position()));
    //cout << kinematics_ << endl;

    float clutch = getRaceClutch(cs);
    control.setAccel(accel);
    control.setBrake(brake);
    control.setGear(gear);
    control.setSteer(steer);
    control.setClutch(clutch);

    static int render = 0;
    if (render++ >= 200) {
        render = 0;
        map_.render_map();
    }
    Mat histImage(400, 400, CV_8UC1, Scalar(255, 255, 255));
    for (int i = 0; i < 19; i++) {
        line(histImage, Point(2*i, 0), Point(2*(i), (int)cs.getTrack(i)), Scalar(0,0,0), 1, 8, 0);
    }
    namedWindow("Image", CV_WINDOW_AUTOSIZE);
    imshow("Image", histImage);
    waitKey(1);
    return control;
}

float SkylakeDriver::getSteer(CarState &cs) {
    double targetAngle = (cs.getAngle() - 0.5 * cs.getTrackPos());
    /*
    int max_id = -1;
    for (int i = 0; i < 19; i++) {
        if (max_id == -1 || cs.getTrack(max_id) <= cs.getTrack(i))
            max_id = i;
    }
    double targetAngle = (cs.getAngle() - ((lrf_angles_[max_id] * M_PI) / 180.0) );// * 0.5);
    */
    return targetAngle / SkylakeConsts::STEER_LOCK_RAD;
}
