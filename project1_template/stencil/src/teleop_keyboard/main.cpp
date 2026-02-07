#include "rix/ipc/fifo.hpp"
#include "rix/ipc/file.hpp"
#include "rix/ipc/signal.hpp"
#include "rix/msg/geometry/Twist2DStamped.hpp"
#include "rix/msg/standard/UInt32.hpp"
#include "rix/util/argument_parser.hpp"
#include "rix/util/time.hpp"
#include "teleop_keyboard/teleop_keyboard.hpp"

using namespace rix::ipc;
using namespace rix::msg;
using namespace rix::util;

int main(int argc, char **argv) {
    ArgumentParser parser("teleop_keyboard",
                          "Sends drive commands to stdout corresponding to characters written to FIFO.");
    parser.add<double>("linear_speed", "Linear speed to drive the MBot (m/s)", 'l', 0.25);
    parser.add<double>("angular_speed", "Angular speed to drive the MBot (rad/s)", 'a', 1.570796);

    if (!parser.parse(argc, argv)) {
        std::cerr << parser.help() << std::endl;
        return 1;
    }

    double linear_speed;
    if (!parser.get<double>("linear_speed", linear_speed)) {
        std::cerr << "Failed to get linear_speed argument." << std::endl;
        return 1;
    }

    double angular_speed;
    if (!parser.get<double>("angular_speed", angular_speed)) {
        std::cerr << "Failed to get angular_speed argument." << std::endl;
        return 1;
    }

    auto input = std::make_unique<Fifo>("teleop", Fifo::Mode::READ);
    auto output = std::make_unique<File>(STDOUT_FILENO);
    TeleopKeyboard teleop_keyboard(std::move(input), std::move(output), linear_speed, angular_speed);

    auto notif = std::make_unique<Signal>(SIGINT);
    teleop_keyboard.spin(std::move(notif));
}