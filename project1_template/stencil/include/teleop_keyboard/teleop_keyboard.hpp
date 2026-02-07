#pragma once

#include <memory>

#include "rix/ipc/fifo.hpp"
#include "rix/ipc/file.hpp"
#include "rix/ipc/signal.hpp"
#include "rix/msg/geometry/Twist2DStamped.hpp"
#include "rix/msg/standard/UInt32.hpp"
#include "rix/util/argument_parser.hpp"
#include "rix/util/time.hpp"

using namespace rix::ipc;
using namespace rix::msg;
using namespace rix::util;

class TeleopKeyboard {
   public:
    TeleopKeyboard(std::unique_ptr<rix::ipc::interfaces::IO> input,
                   std::unique_ptr<rix::ipc::interfaces::IO> output, double linear_speed,
                   double angular_speed);

    void spin(std::unique_ptr<rix::ipc::interfaces::Notification> notif);

   private:
    std::unique_ptr<rix::ipc::interfaces::IO> input;
    std::unique_ptr<rix::ipc::interfaces::IO> output;
    double linear_speed;
    double angular_speed;
};