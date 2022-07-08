#include "lidarkit.hpp"

#include <errno.h>
#include <exception>
#include <fcntl.h>
#include <iomanip>
#include <iostream>
#include <optional>
#include <termios.h>
#include <unistd.h>
#include <vector>

LidarKit::LidarKit(std::string dev_uri) : dev_uri(dev_uri), is_running(false)
{
    open_device();
    if (this->fd == -1) throw std::exception();
}

void LidarKit::open_device()
{
    // open device file descriptor
    this->fd = open(dev_uri.c_str(), O_RDONLY | O_NOCTTY | O_NDELAY);
    if (this->fd == -1) {
        logger("Unable to open " + dev_uri);
    } else {
        fcntl(this->fd, F_SETFL, 0);
        //fcntl(this->fd, F_SETFL, FNDELAY);

        termios options;
        tcgetattr(this->fd, &options);
        cfsetispeed(&options, B230400); // set baud rate
        options.c_cflag |= (CLOCAL | CREAD); // config for reading
        options.c_cflag &= ~CSIZE;
        options.c_cflag &= ~PARENB;
        options.c_cflag &= ~CSTOPB;
        options.c_cflag |= CS8; // 8-bit chars
        options.c_cc[VTIME] = 10; // 1s timeout
        tcsetattr(this->fd, TCSANOW, &options);
    }
}

void LidarKit::close_device()
{
    close(this->fd);
}

void LidarKit::thread_loop()
{
    using namespace std;

    vector<uint8_t> packet;
    packet.reserve(PACKET_LEN);

    while (this->is_running) {
        ssize_t n;
        // check first byte (magic number)
        while (this->is_running) {
            n = read(this->fd, packet.data(), 1);
            if (n == 0) continue;
            if (packet[0] == 0x54) break;
        }
        // get remaining bytes
        ssize_t bytes_got = 1;
        while (this->is_running && bytes_got < PACKET_LEN) {
            n = read(this->fd, packet.data() + bytes_got, PACKET_LEN - bytes_got);
            logger("Got: " + to_string(n));
            if (n == -1) logger("Error: " + to_string(errno));
            if (n == 0 || n == -1) continue;
            bytes_got += n;
        }

        // read bytes, convert to words
        uint16_t radar_speed_word   = static_cast<uint16_t>(packet[2]) 
                                    + (static_cast<uint16_t>(packet[3])<<8);
        uint16_t start_angle_word   = static_cast<uint16_t>(packet[4]) 
                                    + (static_cast<uint16_t>(packet[5])<<8);
        uint16_t end_angle_word     = static_cast<uint16_t>(packet[42]) 
                                    + (static_cast<uint16_t>(packet[43])<<8);
        uint16_t timestamp_word     = static_cast<uint16_t>(packet[44]) 
                                    + (static_cast<uint16_t>(packet[45])<<8);

        // fix types, properly convert units
        int radar_speed = radar_speed_word;
        double start_angle = start_angle_word / 100.0;
        double end_angle = end_angle_word / 100.0;
        int timestamp = timestamp_word;

        int step = (end_angle - start_angle) / (NUM_POINTS - 1);
        for (int i = 0; i < NUM_POINTS; i++) {
            int j = 6 + 3*i;

            uint16_t this_dist_word = static_cast<uint16_t>(packet[j])
                                    + (static_cast<uint16_t>(packet[j+1])<<8);
        
            double this_dist = this_dist_word / 1000.0;
            int this_conf = packet[j+2];
            double this_angle = start_angle + i*step;

            LidarPoint p(this_angle, this_dist, this_conf, timestamp);
            cout << p << endl;
        }
    }
}

void LidarKit::start()
{
    this->is_running = true;
    this->dev_thread = std::thread([this]() {
        thread_loop();
    });
}

void LidarKit::stop()
{
    this->is_running = false;
    this->dev_thread.join();
}