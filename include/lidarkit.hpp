#pragma once

#include <atomic>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "util.hpp"

const size_t PACKET_LEN = 47;
const size_t NUM_POINTS = 12;

#ifdef __APPLE__
const std::string DEFAULT_DEVICE_URI = "/dev/tty.usbserial-0001";
#else
const std::string DEFAULT_DEVICE_URI = "/dev/ttyUSB0";
#endif

class LidarKit {
private:
    int fd;
    std::string dev_uri;
    std::atomic<bool> is_running;
    std::thread dev_thread;
    std::vector<LidarPoint> points;
    std::mutex points_mtx;

    void open_device();
    void close_device();
    void reset_device();
    void thread_loop();

public:
    LidarKit(std::string dev_uri);
    ~LidarKit();
    bool start();
    void stop();
    std::vector<LidarPoint> get_points();
};