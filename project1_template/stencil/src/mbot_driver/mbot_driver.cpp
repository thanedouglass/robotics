#include "mbot_driver/mbot_driver.hpp"

using namespace rix::ipc;
using namespace rix::msg;

MBotDriver::MBotDriver(std::unique_ptr<interfaces::IO> input, std::unique_ptr<MBotBase> mbot)
    : input(std::move(input)), mbot(std::move(mbot)) {}

void MBotDriver::spin(std::unique_ptr<interfaces::Notification> notif) {
    /* TODO */
}