#include <iostream>

#include "mbot/mbot.hpp"
#include "mbot/mbot_base.hpp"
#include "mbot_driver/mbot_driver.hpp"
#include "rix/ipc/file.hpp"
#include "rix/ipc/interfaces/io.hpp"
#include "rix/ipc/interfaces/notification.hpp"
#include "rix/ipc/signal.hpp"
#include "rix/msg/geometry/Twist2DStamped.hpp"
#include "rix/msg/standard/UInt32.hpp"

using namespace rix::ipc;
using namespace rix::msg;

int main() {
    auto mbot = std::make_unique<MBot>();
    if (!mbot->ok()) {
        return 1;
    }

    auto input = std::make_unique<File>(STDIN_FILENO);
    auto sig = std::make_unique<Signal>(SIGINT);

    MBotDriver driver(std::move(input), std::move(mbot));
    driver.spin(std::move(sig)); 
}