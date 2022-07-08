#pragma once

#include <atomic>
#include <string>
#include <thread>

#include "util.hpp"

const size_t PACKET_LEN = 47;
const size_t NUM_POINTS = 12;

class LidarKit {
private:
    int fd;
    std::string dev_uri;
    std::atomic<bool> is_running;
    std::thread dev_thread;

    void open_device();
    void close_device();
    void reset_device();
    void thread_loop();

public:
    LidarKit(std::string dev_uri);
    ~LidarKit();
    void start();
    void stop();
};