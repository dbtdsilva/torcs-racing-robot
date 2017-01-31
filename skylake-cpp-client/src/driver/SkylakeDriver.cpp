#include "SkylakeDriver.h"
#include "core/SkylakeConsts.h"

/* Gear Changing Constants*/
const int SkylakeDriver::gearUp[6] = {5000, 6000, 6000, 6500, 7000, 0};
const int SkylakeDriver::gearDown[6] = {0, 2500, 3000, 3000, 3500, 3500};

/* Stuck constants*/
const int SkylakeDriver::stuckTime = 25;
const float SkylakeDriver::stuckAngle = .523598775; //PI/6

/* Accel and Brake Constants*/
const float SkylakeDriver::maxSpeedDist = 70;
const float SkylakeDriver::maxSpeed = 200;
const float SkylakeDriver::sin5 = 0.08716;
const float SkylakeDriver::cos5 = 0.99619;

/* Steering constants*/
const float SkylakeDriver::steerLock = 0.366519;
const float SkylakeDriver::steerSensitivityOffset = 80.0;
const float SkylakeDriver::wheelSensitivityCoeff = 1;

/* ABS Filter Constants */
const float SkylakeDriver::wheelRadius[4] = {0.3306, 0.3306, 0.3276, 0.3276};
const float SkylakeDriver::absSlip = 2.0;
const float SkylakeDriver::absRange = 3.0;
const float SkylakeDriver::absMinSpeed = 3.0;

/* Clutch constants */
const float SkylakeDriver::clutchMax = 0.5;
const float SkylakeDriver::clutchDelta = 0.05;
const float SkylakeDriver::clutchRange = 0.82;
const float SkylakeDriver::clutchDeltaTime = 0.02;
const float SkylakeDriver::clutchDeltaRaced = 10;
const float SkylakeDriver::clutchDec = 0.01;
const float SkylakeDriver::clutchMaxModifier = 1.3;
const float SkylakeDriver::clutchMaxTime = 1.5;

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

SkylakeDriver::SkylakeDriver() : control(0, 0, 0, 0, 0), kinematics_() {
    stuck = 0;
    clutch = 0.0;
    //map_.enable_debug();
    //map_.render_map();
}

CarControl SkylakeDriver::wDrive(CarState cs)
{


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
    if (cs.getSpeedX() < 150) {
        accel += 0.1;
    } else {
        accel -= 0.1;
    }
    accel = accel < 0.0 ? 0 : accel;
    accel = accel > 1 ? 1 : accel;

    static double last_distance = 0;
    double distance = cs.getDistRaced() - last_distance;

    kinematics_.update_position(cs);
    map_.increase_visited_counter(M_X(kinematics_.get_position()), M_Y(kinematics_.get_position()));
    //cout << kinematics_ << endl;

    control.setAccel(accel);
    control.setBrake(brake);
    control.setGear(gear);
    control.setSteer(steer);
    control.setClutch(clutch);

    static int render = 0;
    if (render++ >= 200) {
        render = 0;
        cout << "Rendering..." << endl;
        map_.render_map();
        cout << render << endl;
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

int SkylakeDriver::getGear(CarState &cs) {

    int gear = cs.getGear();
    int rpm = cs.getRpm();

    // if gear is 0 (N) or -1 (R) just return 1 
    if (gear < 1)
        return 1;
    // check if the RPM value of car is greater than the one suggested 
    // to shift up the gear from the current one     
    if (gear < 6 && rpm >= gearUp[gear - 1])
        return gear + 1;
    else
        // check if the RPM value of car is lower than the one suggested
        // to shift down the gear from the current one
    if (gear > 1 && rpm <= gearDown[gear - 1])
        return gear - 1;
    else // otherwhise keep current gear
        return gear;
}

float SkylakeDriver::getSteer(CarState &cs) {
    int max_id = -1;
    for (int i = 0; i < 19; i++) {
        if (max_id == -1 || cs.getTrack(max_id) <= cs.getTrack(i))
            max_id = i;
    }
    // steering angle is compute by correcting the actual car angle w.r.t. to track
    // axis [cs.getAngle()] and to adjust car position w.r.t to middle of track [cs.getTrackPos()*0.5]
    printf("%7.4f\n", cs.getAngle());
    double targetAngle = (cs.getAngle() - ((lrf_angles_[max_id] * M_PI) / 180.0) );// * 0.5);
    // at high speed reduce the steering command to avoid loosing the control
    //if (cs.getSpeedX() > steerSensitivityOffset)
    //    return targetAngle / (steerLock * (cs.getSpeedX() - steerSensitivityOffset) * wheelSensitivityCoeff);
    //else
    return targetAngle / SkylakeConsts::STEER_LOCK_RAD;
}

float SkylakeDriver::getAccel(CarState &cs) {
    // checks if car is out of track
    if (cs.getTrackPos() < 1 && cs.getTrackPos() > -1) {
        // reading of sensor at +5 degree w.r.t. car axis
        float rxSensor = cs.getTrack(10);
        // reading of sensor parallel to car axis
        float cSensor = cs.getTrack(9);
        // reading of sensor at -5 degree w.r.t. car axis
        float sxSensor = cs.getTrack(8);

        float targetSpeed;

        // track is straight and enough far from a turn so goes to max speed
        if (cSensor > maxSpeedDist || (cSensor >= rxSensor && cSensor >= sxSensor))
            targetSpeed = maxSpeed;
        else {
            // approaching a turn on right
            if (rxSensor > sxSensor) {
                // computing approximately the "angle" of turn
                float h = cSensor * sin5;
                float b = rxSensor - cSensor * cos5;
                float sinAngle = b * b / (h * h + b * b);
                // estimate the target speed depending on turn and on how close it is
                targetSpeed = maxSpeed * (cSensor * sinAngle / maxSpeedDist);
            }
                // approaching a turn on left
            else {
                // computing approximately the "angle" of turn
                float h = cSensor * sin5;
                float b = sxSensor - cSensor * cos5;
                float sinAngle = b * b / (h * h + b * b);
                // estimate the target speed depending on turn and on how close it is
                targetSpeed = maxSpeed * (cSensor * sinAngle / maxSpeedDist);
            }

        }

        // accel/brake command is expontially scaled w.r.t. the difference between target speed and current one
        return 2 / (1 + exp(cs.getSpeedX() - targetSpeed)) - 1;
    } else
        return 0.3; // when out of track returns a moderate acceleration command

}

float SkylakeDriver::filterABS(CarState &cs, float brake) {
    // convert speed to m/s
    float speed = cs.getSpeedX() / 3.6;
    // when spedd lower than min speed for abs do nothing
    if (speed < absMinSpeed)
        return brake;

    // compute the speed of wheels in m/s
    float slip = 0.0f;
    for (int i = 0; i < 4; i++) {
        slip += cs.getWheelSpinVel(i) * wheelRadius[i];
    }
    // slip is the difference between actual speed of car and average speed of wheels
    slip = speed - slip / 4.0f;
    // when slip too high applu ABS
    if (slip > absSlip) {
        brake = brake - (slip - absSlip) / absRange;
    }

    // check brake is not negative, otherwise set it to zero
    if (brake < 0)
        return 0;
    else
        return brake;
}

void SkylakeDriver::onShutdown() {
    cout << "Bye bye!" << endl;
}

void SkylakeDriver::onRestart() {
    cout << "Restarting the race!" << endl;
}

void SkylakeDriver::clutching(CarState &cs, float &clutch) {
    double maxClutch = clutchMax;

    // Check if the current situation is the race start
    if (cs.getCurLapTime() < clutchDeltaTime && stage == RACE && cs.getDistRaced() < clutchDeltaRaced)
        clutch = maxClutch;

    // Adjust the current value of the clutch
    if (clutch > 0) {
        double delta = clutchDelta;
        if (cs.getGear() < 2) {
            // Apply a stronger clutch output when the gear is one and the race is just started
            delta /= 2;
            maxClutch *= clutchMaxModifier;
            if (cs.getCurLapTime() < clutchMaxTime)
                clutch = maxClutch;
        }

        // check clutch is not bigger than maximum values
        clutch = min(maxClutch, double(clutch));

        // if clutch is not at max value decrease it quite quickly
        if (clutch != maxClutch) {
            clutch -= delta;
            clutch = max(0.0, double(clutch));
        } // if clutch is at max value decrease it very slowly
        else {
            clutch -= clutchDec;
        }
    }
}

void SkylakeDriver::init(float *angles) {
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
