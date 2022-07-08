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

const uint8_t crc_table[256] =
{
    0x00, 0x4d, 0x9a, 0xd7, 0x79, 0x34, 0xe3,
    0xae, 0xf2, 0xbf, 0x68, 0x25, 0x8b, 0xc6, 0x11, 0x5c, 0xa9, 0xe4, 0x33,
    0x7e, 0xd0, 0x9d, 0x4a, 0x07, 0x5b, 0x16, 0xc1, 0x8c, 0x22, 0x6f, 0xb8,
    0xf5, 0x1f, 0x52, 0x85, 0xc8, 0x66, 0x2b, 0xfc, 0xb1, 0xed, 0xa0, 0x77,
    0x3a, 0x94, 0xd9, 0x0e, 0x43, 0xb6, 0xfb, 0x2c, 0x61, 0xcf, 0x82, 0x55,
    0x18, 0x44, 0x09, 0xde, 0x93, 0x3d, 0x70, 0xa7, 0xea, 0x3e, 0x73, 0xa4,
    0xe9, 0x47, 0x0a, 0xdd, 0x90, 0xcc, 0x81, 0x56, 0x1b, 0xb5, 0xf8, 0x2f,
    0x62, 0x97, 0xda, 0x0d, 0x40, 0xee, 0xa3, 0x74, 0x39, 0x65, 0x28, 0xff,
    0xb2, 0x1c, 0x51, 0x86, 0xcb, 0x21, 0x6c, 0xbb, 0xf6, 0x58, 0x15, 0xc2,
    0x8f, 0xd3, 0x9e, 0x49, 0x04, 0xaa, 0xe7, 0x30, 0x7d, 0x88, 0xc5, 0x12,
    0x5f, 0xf1, 0xbc, 0x6b, 0x26, 0x7a, 0x37, 0xe0, 0xad, 0x03, 0x4e, 0x99,
    0xd4, 0x7c, 0x31, 0xe6, 0xab, 0x05, 0x48, 0x9f, 0xd2, 0x8e, 0xc3, 0x14,
    0x59, 0xf7, 0xba, 0x6d, 0x20, 0xd5, 0x98, 0x4f, 0x02, 0xac, 0xe1, 0x36,
    0x7b, 0x27, 0x6a, 0xbd, 0xf0, 0x5e, 0x13, 0xc4, 0x89, 0x63, 0x2e, 0xf9,
    0xb4, 0x1a, 0x57, 0x80, 0xcd, 0x91, 0xdc, 0x0b, 0x46, 0xe8, 0xa5, 0x72,
    0x3f, 0xca, 0x87, 0x50, 0x1d, 0xb3, 0xfe, 0x29, 0x64, 0x38, 0x75, 0xa2,
    0xef, 0x41, 0x0c, 0xdb, 0x96, 0x42, 0x0f, 0xd8, 0x95, 0x3b, 0x76, 0xa1,
    0xec, 0xb0, 0xfd, 0x2a, 0x67, 0xc9, 0x84, 0x53, 0x1e, 0xeb, 0xa6, 0x71,
    0x3c, 0x92, 0xdf, 0x08, 0x45, 0x19, 0x54, 0x83, 0xce, 0x60, 0x2d, 0xfa,
    0xb7, 0x5d, 0x10, 0xc7, 0x8a, 0x24, 0x69, 0xbe, 0xf3, 0xaf, 0xe2, 0x35,
    0x78, 0xd6, 0x9b, 0x4c, 0x01, 0xf4, 0xb9, 0x6e, 0x23, 0x8d, 0xc0, 0x17,
    0x5a, 0x06, 0x4b, 0x9c, 0xd1, 0x7f, 0x32, 0xe5, 0xa8
};

uint8_t calc_crc8(uint8_t *p, uint8_t len)
{
    uint8_t crc = 0;
    uint16_t i;
    for (i = 0; i < len; i++)
    {
    crc = crc_table[(crc ^ *p++) & 0xff];
    }
    return crc;
}

LidarKit::LidarKit(std::string dev_uri) : dev_uri(dev_uri), is_running(false)
{
    this->open_device();
    if (this->fd == -1) throw std::exception();
}

LidarKit::~LidarKit()
{
    this->stop();
    this->close_device();
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
        cfsetospeed(&options, B230400); // set baud rate
        options.c_cflag |= (CLOCAL | CREAD); // config for reading
        options.c_cflag &= ~CSIZE; // clear size flags
        options.c_cflag &= ~PARENB; // no parity bit
        options.c_cflag &= ~CSTOPB; // 1 stop bit
        options.c_cflag |= CS8; // 8-bit chars
        //options.c_cc[VMIN] = 1;
        options.c_cc[VTIME] = 10; // 1s timeout
        tcsetattr(this->fd, TCSANOW, &options);
    }
}

void LidarKit::close_device()
{
    close(this->fd);
}

void LidarKit::reset_device()
{
    tcflush(this->fd, TCIOFLUSH);
}

void LidarKit::thread_loop()
{
    using namespace std;

    vector<uint8_t> packet(PACKET_LEN, 0);

    this->reset_device();
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
            if (n == -1) logger("Error: " + to_string(errno));
            if (n == 0 || n == -1) continue;
            bytes_got += n;
        }

        if(calc_crc8(packet.data(), 46) != packet[46]) {
            logger("Bad checksum, skipping...");
            continue;
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

        // calculate step, with correction
        double step = (end_angle >= start_angle)
                    ? (end_angle - start_angle) / (NUM_POINTS - 1)
                    : ((end_angle + 360.0) - start_angle) / (NUM_POINTS - 1);
        
        // parse intermediate points
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
    if (!this->is_running) {
        this->is_running = true;
        this->dev_thread = std::thread([this]() {
            thread_loop();
        });
    }
}

void LidarKit::stop()
{
    if (this->is_running) {
        this->is_running = false;
        this->dev_thread.join();
    }
}