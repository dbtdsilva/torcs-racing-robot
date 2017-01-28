#ifndef BASEDRIVER_H_
#define BASEDRIVER_H_

#include <iostream>

using namespace std;

class BaseDriver {
public:
    typedef enum {
        WARMUP, QUALIFYING, RACE, UNKNOWN
    } tstage;
    tstage stage;
    char trackName[100];

    // Default Constructor
    BaseDriver() {};
    // Default Destructor
    virtual ~BaseDriver() {};

    // Initialization of the desired angles for the rangefinders
    virtual void init(float *angles) {
        for (int i = 0; i < 19; ++i)
            angles[i] = -90 + i * 10;
    };

    // The main function:
    //     - the input variable sensors represents the current world sate
    //     - it returns a string representing the controlling action to perform
    virtual string drive(string sensors) = 0;
    // Callback function called at shutdown
    virtual void onShutdown() {};
    // Callback function called at server restart
    virtual void onRestart() {};
};

#endif /*BASEDRIVER_H_*/
