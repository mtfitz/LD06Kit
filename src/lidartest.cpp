#include <chrono>
#include <iostream>
#include <optional>
#include <signal.h>
#include <thread>
#include <vector>

#include "lidarkit.hpp"

static bool is_running = true;

using namespace std;

void sigint_handler(int)
{
    is_running = false;
    cout << "Done." << endl;
}

int main(int argc, char** argv)
{
    // register signal handler, for CTRL+C interrupt
    signal(SIGINT, sigint_handler);

    LidarKit lk(DEFAULT_DEVICE_URI);
    lk.start();
    while (is_running) {
        auto v = lk.get_points();
        cout << "Size: " << v.size() << endl;
        this_thread::sleep_for(0.2s);
    }
    lk.stop();

    return 0;
}