#ifndef WRAPPERBASEDRIVER_H_
#define WRAPPERBASEDRIVER_H_

#include "Base/CarState.h"
#include "Base/CarControl.h"
#include "Base/BaseDriver.h"
#include <cmath>
#include <cstdlib>

class WrapperBaseDriver : public BaseDriver
{
public:
	// the drive function wiht string input and output
	string drive(string sensors);
	// drive function that exploits the CarState and CarControl wrappers as input and output.
	virtual CarControl wDrive(CarState cs) = 0;
};

#endif /*WRAPPERBASEDRIVER_H_*/
