#ifndef WRAPPERBASEDRIVER_H_
#define WRAPPERBASEDRIVER_H_

#include "CarState.h"
#include "CarControl.h"
#include "BaseDriver.h"
#include <cmath>
#include <cstdlib>

class WrapperBaseDriver : public BaseDriver
{
public:
	// the drive function wiht string input and output
	virtual string drive(string sensors);
	// drive function that exploits the CarState and CarControl wrappers as input and output.
	virtual CarControl wDrive(CarState cs) = 0;
};

#endif /*WRAPPERBASEDRIVER_H_*/
