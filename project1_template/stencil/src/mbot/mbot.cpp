#include "mbot/mbot.hpp"

MBot::MBot() : file("/dev/mbot_lcm", O_RDWR | O_NOCTTY | O_NDELAY, 0) {
    if (!file.ok()) {
        perror("open");
        return;
    }
    // Set up the serial port
    int fd = file.fd();
    struct termios options;
    tcgetattr(fd, &options);
    cfsetspeed(&options, B115200);
    options.c_cflag &= ~(CSIZE | PARENB | CSTOPB | CRTSCTS);
    options.c_cflag |= CS8 | CREAD | CLOCAL;
    options.c_oflag &= ~OPOST;
    options.c_lflag &= ~(ICANON | ISIG | ECHO | IEXTEN); /* Set non-canonical mode */
    options.c_cc[VTIME] = 1;
    options.c_cc[VMIN] = 0;
    cfmakeraw(&options);
    tcflush(fd, TCIFLUSH);
    tcsetattr(fd, TCSANOW, &options);
    if (tcgetattr(fd, &options) != 0) {
        close(fd);
        return;
    }

    timesync_thr = std::thread(std::bind(&MBot::timesync, this));
}

MBot::~MBot() {
    // Set the stop flag for the time synchronization thread
    stop_timesync_flag = 1;

    // Join the time synchronization thread
    if (timesync_thr.joinable()) {
        timesync_thr.join();
    }
}

bool MBot::ok() const { return file.ok(); }

void MBot::drive(const Twist2DStamped &cmd) const {
    serial_twist2D_t mbot_cmd;
    mbot_cmd.utime = rix::util::Time(cmd.header.stamp).to_microseconds();
    mbot_cmd.vx = cmd.twist.vx;
    mbot_cmd.vy = cmd.twist.vy;
    mbot_cmd.wz = cmd.twist.wz;

    // Encode the drive command
    const size_t msg_size = sizeof(serial_twist2D_t) + ROS_PKG_LENGTH;
    uint8_t msg[msg_size];
    encode_msg(reinterpret_cast<uint8_t *>(&mbot_cmd), sizeof(serial_twist2D_t), MBOT_VEL_CMD, msg, msg_size);

    // Send the drive command
    mtx.lock();
    file.write(msg, sizeof(msg));
    mtx.unlock();
}

void MBot::timesync() {
    int status;

    // Time synchronization loop
    while (!stop_timesync_flag) {
        // Encode the timesync message
        serial_timestamp_t msg = {0};
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        msg.utime = ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
        const size_t msg_size = sizeof(serial_timestamp_t) + ROS_PKG_LENGTH;
        uint8_t rospkt[msg_size];
        if (encode_msg((uint8_t *)&msg, sizeof(serial_timestamp_t), MBOT_TIMESYNC, rospkt, msg_size) < 0) {
            perror("encode_msg");
            break;
        }

        // Send the timesync message
        mtx.lock();
        status = file.write(rospkt, msg_size);
        mtx.unlock();
        if (status < 0) {
            perror("write");
            break;
        }

        // Run at 2 Hz
        usleep(500000);
    }
}