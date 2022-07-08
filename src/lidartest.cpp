#include <chrono>
#include <iostream>
#include <optional>
#include <thread>
#include <vector>

#include "lidarkit.hpp"

#ifdef __APPLE__
const std::string DEFAULT_DEVICE_URI = "/dev/tty.usbserial-0001";
#else
const std::string DEFAULT_DEVICE_URI = "/dev/ttyUSB0";
#endif

int main()
{
    using namespace std;

    LidarKit lk(DEFAULT_DEVICE_URI);
    lk.start();
    this_thread::sleep_for(5s);
    lk.stop();

    return 0;
}