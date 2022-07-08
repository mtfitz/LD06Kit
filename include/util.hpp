#pragma once

#include <iomanip>
#include <string>

void logger(std::string msg);

struct LidarPoint {
    float angle;
    float distance;
    int confidence;
    int timestamp;

    LidarPoint(float angle, float dist, int conf=-1, int ts=-1) :
        angle(angle), distance(dist), confidence(conf), timestamp(ts) {}
};

std::ostream& operator<<(std::ostream& os, LidarPoint& p);