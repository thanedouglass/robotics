#pragma once

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

#include <mutex>
#include <thread>
#include <functional>

#include "mbot/messages.hpp"
#include "mbot/mbot_base.hpp"
#include "rix/ipc/file.hpp"
#include "rix/msg/geometry/Twist2DStamped.hpp"

using rix::msg::geometry::Twist2DStamped;

class MBot : public MBotBase {
   public:
    MBot();
    ~MBot();

    bool ok() const;
    void drive(const Twist2DStamped &cmd) const;

   private:
    void timesync();

    mutable std::mutex mtx;
    std::thread timesync_thr;
    std::atomic<bool> stop_timesync_flag;
    rix::ipc::File file;
};