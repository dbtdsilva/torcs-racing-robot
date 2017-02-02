#ifndef FUZZYRIVER_H_
#define FUZZYRIVER_H_

#include "core-common/WrapperBaseDriver.h"

#include <fl/Headers.h>
#include <core/SkylakeBaseDriver.h>

class FuzzyDriver : public SkylakeBaseDriver {
public:
    FuzzyDriver();
    ~FuzzyDriver();

    bool loadFCLfile(std::string fcFile);
    virtual CarControl wDrive(CarState cs);
private:
    std::string fclFile;
    fl::Engine *flEngine;
};

#endif /*FUZZYRIVER_H_*/
