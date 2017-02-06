#include "VFHFuzzyDriver.h"

#include <iomanip>
#include <core/SkylakeConsts.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;

VFHFuzzyDriver::VFHFuzzyDriver()
{
    this->control_ = CarControl(0, 0, 0, 0, 0);
}

VFHFuzzyDriver::~VFHFuzzyDriver()
{
    delete flEngine;
}

bool VFHFuzzyDriver::loadFCLfile(std::string fclFile)
{
    bool fileLoaded = false;

    try {
        flEngine = fl::FclImporter().fromFile(fclFile);
        fileLoaded = true;
    } catch (fl::Exception &e) {
        std::cerr << "FuzzyLite: " << e.getWhat() << std::endl;
    }

    return fileLoaded;
}

CarControl VFHFuzzyDriver::wDrive(CarState cs)
{
    int gear = 0; // {-1,.., 6}
    float steer = 0; // [-1, 1]
    int meta = 0; // {0, 1}
    int focus = 0; // [-90, 90]
    float accel = 0; // [0, 1]
    float brake = 0; // [0, 1]

    float trackPos = cs.getTrackPos(); // [-1, 1]
    float angle = cs.getAngle(); // [-3.15, 3.15]
    float speedX = cs.getSpeedX(); // [0, inf]

    int max_id = -1;
    for (int i = 0; i < 19; i++) {
        if (max_id == -1 || cs.getTrack(max_id) <= cs.getTrack(i))
            max_id = i;
    }
    double max_angle = -(lrf_angles_[max_id] * M_PI) / 180.0; // [-3.15, 3.15]
    float dist_max_angle = cs.getTrack(max_id);

    double diff_angle = abs(max_angle - angle);

    flEngine->setInputValue("angle", angle);
    flEngine->setInputValue("maxAngle", max_angle);
    flEngine->setInputValue("distFront", cs.getTrack(9));
    flEngine->setInputValue("distMaxAngle", dist_max_angle);
    flEngine->setInputValue("trackPos", trackPos);
    flEngine->setInputValue("speedX", speedX);

    flEngine->process();

    accel = (float)flEngine->getOutputVariable("accel")->getOutputValue();
    brake = (float)flEngine->getOutputVariable("brake")->getOutputValue();

    gear = getGear(cs);
    steer = getSteer(cs, cs.getTrack(9) <= 0);

    if (cs.getTrack(9) <= 70)
        steer *= 2.0;
    if (cs.getTrack(9) <= 70)
        brake *= 4;

    brake = brake < 0 ? 0 : brake > 1 ? 1 : brake;
    accel = accel < 0 ? 0 : accel > 1 ? 1 : accel;
    steer = steer < -1 ? -1 : steer > 1 ? 1 : steer;

    float filtered_brake = filterABS(cs, brake);
    float filtered_accel = filterTCL(cs, accel);

    //if (cs.getSpeedX() >= 140)
    //   filtered_accel = 0;
    //if (cs.getSpeedX() <= 50)
    //    steer /= 3;

    static int cycle = 0;
    std::cout << std::fixed << std::setw(7) << std::setprecision(5) << std::setfill('0')
              << "cycle: " << cycle++ << ", distRaced: " << cs.getDistRaced() << ", trackPos: " << trackPos
              << ", speedX: " << speedX << ", angle: " << angle << ", maxAngle: " << max_angle << ", diffAngle: "
              << diff_angle << ", steer: " << steer << ", accel: " << accel << ", filtered accel: "
              << filtered_accel << ", brake: " << brake << ", filtered brake: "
              << filtered_brake << ", distFront: " << cs.getTrack(9) << ", distMaxAngle: "
              << dist_max_angle << std::endl;

    control_.setAccel(filtered_accel);
    control_.setBrake(filtered_brake);
    control_.setGear(gear);
    control_.setSteer(steer);
    control_.setClutch(getRaceClutch(cs));
    control_.setFocus(focus);
    control_.setMeta(meta);

    /* Used to visualize the VFH */
    Mat histImage(400, 400, CV_8UC1, Scalar(255, 255, 255));
    for (int i = 0; i < 19; i++) {
        line(histImage, Point(2*i*8, 0), Point(2*(i)*8, (int)cs.getTrack(i) * 2), Scalar(0,0,0), 8, 8, 0);
    }
    namedWindow("Image", CV_WINDOW_AUTOSIZE);
    imshow("Image", histImage);
    waitKey(1);
    return control_;
}

float interpolation(float x) {
    // Really aggressive
    /*return  (3.819181134387*pow(10.0,-16)*pow(x,9) - 3.4714224102966*pow(10.0,-13)*pow(x,8) +
            1.2506847536199*pow(10.0,-10)*pow(x,7) - 2.3163334050356*pow(10.0,-8)*pow(x,6) +
            2.3875280480966*pow(10.0,-6)*pow(x,5) - 0.000138234*pow(x,4) + 0.0042882*pow(x,3)-0.0612592*pow(x,2) +
            0.100325* x+10)*0.5; */
    return static_cast<float>(2.6825396825396 * pow(10.0,-10) * pow(x,5) - 1.5146031746031 * pow(10.0,-7) * pow(x,4) +
            0.0000305389 * pow(x,3) - 0.00249325 * pow(x,2) + 0.0455714 * x + 3.0);

}

float VFHFuzzyDriver::getSteer(CarState &cs, bool lost) {
    double targetAngle;
    int max_id = -1;
    for (int i = 0; i < 19; i++) {
        if (max_id == -1 || cs.getTrack(max_id) <= cs.getTrack(i))
            max_id = i;
    }
    if (lost || cs.getTrack(9) >= 150.0) {
        targetAngle = (cs.getAngle() - 0.1 * cs.getTrackPos());
    } else {
        targetAngle = ((cs.getAngle() - ((lrf_angles_[max_id] * M_PI) / 180.0))
                      * (interpolation(cs.getSpeedX())));
    }
    return targetAngle / SkylakeConsts::STEER_LOCK_RAD;
}

float VFHFuzzyDriver::filterABS(CarState &cs, float brake)
{
    if (cs.getSpeedX() < 15.0)
        return brake;

    // compute the speed of wheels in m/s
    double slip = 0.0;
    for (int i = 0; i < 4; i++) {
        if (i < 2)
            slip += cs.getWheelSpinVel(i) * SkylakeConsts::WHEEL_FRONT_RADIUS;
        else
            slip += cs.getWheelSpinVel(i) * SkylakeConsts::WHEEL_BACK_RADIUS;
    }
    double speed = cs.getSpeedX() / 3.6;
    // slip is the difference between actual speed of car and average speed of wheels
    slip = speed - slip / 4.0;

    if (slip > 2.0) {
        brake = brake - (slip - 2.0) / 3.0;
    }

    // check brake is not negative, otherwise set it to zero
    if (brake < 0)
        return 0;
    else
        return brake;
}

float VFHFuzzyDriver::filterTCL(CarState &cs, float accel) {
    if (cs.getSpeedX() < 15.0) return accel;
    double slip = cs.getSpeedX() / ((cs.getWheelSpinVel(2) + cs.getWheelSpinVel(3)) *
            SkylakeConsts::WHEEL_BACK_RADIUS / 2.0);
    if (slip < 0.9)
        accel = 0.0;
    return accel;
}