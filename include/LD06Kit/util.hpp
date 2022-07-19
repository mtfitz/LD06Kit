#pragma once

#include <iomanip>
#include <string>

void logger(std::string msg);

struct LidarPoint {
    float angle;        ///< point angle in degrees
    float distance;     ///< point distance in meters
    int confidence;     ///< confidence of reading (generally >200 is OK)
    int timestamp;      ///< timestamp in milliseconds (resets after 65536 ms)

    LidarPoint(float angle, float dist, int conf=-1, int ts=-1) :
        angle(angle), distance(dist), confidence(conf), timestamp(ts) {}
};

///
/// For printing LidarPoint structures to C++ streams.
///
std::ostream& operator<<(std::ostream& os, LidarPoint& p);