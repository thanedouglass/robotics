#include "mbot_driver/mbot_driver.hpp"
#include <iostream>

using namespace rix::ipc;
using namespace rix::msg;

MBotDriver::MBotDriver(std::unique_ptr<interfaces::IO> input,
                       std::unique_ptr<MBotBase> mbot)
    : input(std::move(input)), mbot(std::move(mbot)) {}

void MBotDriver::spin(std::unique_ptr<interfaces::Notification> notif) {
  rix::util::Duration timeout(0, 100000000); // 100ms timeout
  std::vector<uint8_t> buffer;

  while (true) {
    // Check for SIGINT with short timeout
    if (notif->wait(timeout)) {
      // SIGINT received, stop the mbot
      geometry::Twist2DStamped stop_cmd;
      stop_cmd.twist.vx = 0.0;
      stop_cmd.twist.vy = 0.0;
      stop_cmd.twist.wz = 0.0;
      mbot->drive(stop_cmd);
      break;
    }

    // Read size prefix (4-byte UInt32)
    standard::UInt32 msg_size;
    uint8_t size_buf[4];
    ssize_t bytes_read = input->read(size_buf, 4);

    if (bytes_read == 0) {
      // EOF reached, stop the mbot
      geometry::Twist2DStamped stop_cmd;
      stop_cmd.twist.vx = 0.0;
      stop_cmd.twist.vy = 0.0;
      stop_cmd.twist.wz = 0.0;
      mbot->drive(stop_cmd);
      break;
    }

    if (bytes_read < 0) {
      // Read error, continue
      continue;
    }

    if (bytes_read != 4) {
      // Incomplete size read, continue
      continue;
    }

    // Deserialize the size
    size_t offset = 0;
    if (!rix::msg::detail::deserialize_number(msg_size.data, size_buf, 4,
                                              offset)) {
      continue;
    }

    // Read the message data
    buffer.resize(msg_size.data);
    bytes_read = input->read(buffer.data(), msg_size.data);

    if (bytes_read != static_cast<ssize_t>(msg_size.data)) {
      // Incomplete message read
      continue;
    }

    // Deserialize the Twist2DStamped message
    geometry::Twist2DStamped twist_msg;
    offset = 0;
    if (twist_msg.deserialize(buffer.data(), buffer.size(), offset)) {
      // Log the received drive command
      std::cout << "Received Drive Command: "
                << "vx=" << twist_msg.twist.vx << ", vy=" << twist_msg.twist.vy
                << ", wz=" << twist_msg.twist.wz << std::endl;

      // Command the mbot with the full message
      mbot->drive(twist_msg);
    }
  }
}