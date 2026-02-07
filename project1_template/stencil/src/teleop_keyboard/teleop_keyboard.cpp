#include <teleop_keyboard/teleop_keyboard.hpp>

TeleopKeyboard::TeleopKeyboard(std::unique_ptr<rix::ipc::interfaces::IO> input,
                               std::unique_ptr<rix::ipc::interfaces::IO> output, double linear_speed,
                               double angular_speed)
    : input(std::move(input)), output(std::move(output)), linear_speed(linear_speed), angular_speed(angular_speed) {}

void TeleopKeyboard::spin(std::unique_ptr<rix::ipc::interfaces::Notification> notif) {
    /* TODO */
}