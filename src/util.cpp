#include "LD06Kit/util.hpp"

#include <iostream>

void logger(std::string msg)
{
    std::cout << msg << std::endl;
}

std::ostream& operator<<(std::ostream& os, LidarPoint& p)
{
    return os << std::fixed << std::setprecision(3) << R"({"angle": )" << p.angle << R"(, "distance": )" << p.distance << R"(, "confidence": )" << p.confidence << R"(, "timestamp": )" << p.timestamp << "}";
}