#include <teleop_keyboard/teleop_keyboard.hpp>

TeleopKeyboard::TeleopKeyboard(std::unique_ptr<rix::ipc::interfaces::IO> input,
                               std::unique_ptr<rix::ipc::interfaces::IO> output,
                               double linear_speed, double angular_speed)
    : input(std::move(input)), output(std::move(output)),
      linear_speed(linear_speed), angular_speed(angular_speed) {}

void TeleopKeyboard::spin(
    std::unique_ptr<rix::ipc::interfaces::Notification> notif) {
  // Set input to non-blocking mode
  input->set_nonblocking(true);

  rix::util::Duration timeout(0, 100000000); // 100ms timeout
  std::vector<uint8_t> buffer;

  while (true) {
    // Check for SIGINT with short timeout
    if (notif->wait(timeout)) {
      // SIGINT received, exit
      break;
    }

    // Read single character from teleop FIFO
    char key;
    ssize_t bytes_read = input->read(reinterpret_cast<uint8_t *>(&key), 1);

    if (bytes_read != 1) {
      // No data available or error, continue
      continue;
    }

    // Map key to Twist command
    geometry::Twist2D twist_cmd;
    twist_cmd.vx = 0.0;
    twist_cmd.vy = 0.0;
    twist_cmd.wz = 0.0;

    switch (key) {
    case 'w': // Forward
      twist_cmd.vx = linear_speed;
      break;
    case 's': // Backward
      twist_cmd.vx = -linear_speed;
      break;
    case 'a': // Rotate left
      twist_cmd.wz = angular_speed;
      break;
    case 'd': // Rotate right
      twist_cmd.wz = -angular_speed;
      break;
    case ' ': // Stop (already initialized to 0)
      break;
    default:
      // Unknown key, ignore
      continue;
    }

    // Create Twist2DStamped with current timestamp
    geometry::Twist2DStamped twist_msg;
    twist_msg.header.stamp = rix::util::Time::now().to_msg();
    twist_msg.twist = twist_cmd;

    // Compute message size
    size_t msg_size = twist_msg.size();

    // Create buffer for size prefix + message
    buffer.resize(4 + msg_size);

    // Serialize size prefix (UInt32)
    standard::UInt32 size_msg;
    size_msg.data = static_cast<uint32_t>(msg_size);
    size_t offset = 0;
    size_msg.serialize(buffer.data(), offset);

    // Serialize the Twist2DStamped message
    twist_msg.serialize(buffer.data(), offset);

    // Write to stdout
    output->write(buffer.data(), buffer.size());
  }
}