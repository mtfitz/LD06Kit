#include <chrono>
#include <iostream>
#include <optional>
#include <signal.h>
#include <thread>
#include <vector>

#include "LD06Kit/lidarkit.hpp"

static bool is_running = true;

using namespace std;

void sigint_handler(int)
{
    is_running = false;
}

int main(int argc, char** argv)
{
    // register signal handler, for smooth CTRL+C interrupt
    signal(SIGINT, sigint_handler);

    string uri = DEFAULT_DEVICE_URI;
    if (argc > 1)
        uri = argv[1];

    try {
        LidarKit lk(uri);
        lk.start();
        this_thread::sleep_for(0.1s);
        while (is_running) {
            auto v = lk.get_points();
            for (auto& p : v) cout << p << endl;
            this_thread::sleep_for(0.5s);
        }
        lk.stop();
	auto v = lk.get_points();
        for (auto& p : v) cout << p << endl;
    } catch (exception& e) {
        cout << "Fatal error!" << endl;
    }

    return 0;
}
