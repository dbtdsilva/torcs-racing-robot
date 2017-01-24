#ifndef SIMPLEPARSER_H_
#define SIMPLEPARSER_H_

#include <iostream>
#include <string>

using namespace std;

class SimpleParser {
public:
    static bool parse(string sensors, string tag, float &value);
    static bool parse(string sensors, string tag, int &value);
    static bool parse(string sensors, string tag, float *value, int size);
    static string stringify(string tag, float value);
    static string stringify(string tag, int value);
    static string stringify(string tag, float *value, int size);
};

#endif
