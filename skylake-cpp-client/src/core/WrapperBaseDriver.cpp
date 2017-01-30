#include "WrapperBaseDriver.h"

string WrapperBaseDriver::drive(string sensors)
{
	CarState cs(sensors);
	CarControl cc = wDrive(cs);
	return cc.toString();	
}


