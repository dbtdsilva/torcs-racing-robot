#ifndef CARSTATE_H_
#define CARSTATE_H_

#define FOCUS_SENSORS_NUM 5
#define TRACK_SENSORS_NUM 19
#define OPPONENTS_SENSORS_NUM 36

#include <iostream>
#include <sstream>
#include <cstring>
#include <cassert>
#include "SimpleParser.h"

using namespace std;

class CarState {
public:
    CarState() {};
    CarState(string sensors);
    string toString();

    /* Getter and setter methods */
    float getAngle();
    float getCurLapTime();
    float getDamage();
    float getDistFromStart();
    float getDistRaced();
    float getFocus(int i);
    float getFuel();
    int getGear();
    float getOpponents(int i);
    float getLastLapTime();
    int getRacePos();
    int getRpm();
    float getSpeedX();
    float getSpeedY();
    float getSpeedZ();
    float getTrack(int i);
    float getTrackPos();
    float getWheelSpinVel(int i);
    float getZ();

    void setAngle(float angle);
    void setCurLapTime(float curLapTime);
    void setDamage(float damage);
    void setDistFromStart(float distFromStart);
    void setDistRaced(float distRaced);
    void setFocus(int i, float value);
    void setFuel(float fuel);
    void setGear(int gear);
    void setLastLapTime(float lastLapTime);
    void setOpponents(int i, float value);
    void setRacePos(int racePos);
    void setRpm(int rpm);
    void setSpeedX(float speedX);
    void setSpeedY(float speedY);
    void setSpeedZ(float speedZ);
    void setTrack(int i, float value);
    void setTrackPos(float trackPos);
    void setWheelSpinVel(int i, float value);
    void setZ(float z);

private:
    float angle;
    float curLapTime;
    float damage;
    float distFromStart;
    float distRaced;
    float focus[FOCUS_SENSORS_NUM];
    float fuel;
    int gear;
    float lastLapTime;
    float opponents[OPPONENTS_SENSORS_NUM];
    int racePos;
    int rpm;
    float speedX;
    float speedY;
    float speedZ;
    float track[TRACK_SENSORS_NUM];
    float trackPos;
    float wheelSpinVel[4];
    float z;
};

#endif /*CARSTATE_H_*/
