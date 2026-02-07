#pragma once

#include <memory>

#include "mbot/mbot.hpp"
#include "mbot/mbot_base.hpp"
#include "rix/ipc/file.hpp"
#include "rix/ipc/interfaces/io.hpp"
#include "rix/ipc/interfaces/notification.hpp"
#include "rix/ipc/signal.hpp"
#include "rix/msg/geometry/Twist2DStamped.hpp"
#include "rix/msg/standard/UInt32.hpp"

using namespace rix::ipc;
using namespace rix::msg;

class MBotDriver {
   public:
    MBotDriver(std::unique_ptr<interfaces::IO> input, std::unique_ptr<MBotBase> mbot);
    void spin(std::unique_ptr<interfaces::Notification> notif);

   private:
    std::unique_ptr<interfaces::IO> input;
    std::unique_ptr<MBotBase> mbot;
};