#include "rix/ipc/signal.hpp"
#include <csignal>
#include <stdexcept>
#include <unistd.h>

namespace rix {
namespace ipc {

std::array<Signal::SignalNotifier, 32> Signal::notifier = {};

/**
 * Initializes the signal handler and the notification pipe.
 */
Signal::Signal(int signum) : signum_(-1) {
  // Validate signum range [1, 32]
  if (signum < 1 || signum > 32) {
    throw std::invalid_argument("Signal number must be between 1 and 32");
  }

  // Set the adjusted signal number (0-indexed)
  signum_ = signum - 1;

  // Check if this signal is already initialized (duplicate registration)
  if (notifier[signum_].is_init) {
    throw std::invalid_argument(
        "Signal handler already initialized for this signal");
  }

  // Initialize the notifier
  notifier[signum_].pipe = Pipe::create(); // Create 2 Pipes (read/write)
  notifier[signum_].is_init = true;

  // Register the static handler for this signal [cite: 74]
  ::signal(signum, Signal::handler);
}

Signal::~Signal() {
  // Reset signal handler and clean up if this is a valid signal
  if (signum_ >= 0 && signum_ < 32 && notifier[signum_].is_init) {
    // Reset the signal handler to default
    ::signal(signum_ + 1, SIG_DFL);
    // Mark as uninitialized so it can be reused
    notifier[signum_].is_init = false;
  }
}

Signal::Signal(Signal &&other) : signum_(-1) {
  std::swap(signum_, other.signum_);
}

Signal &Signal::operator=(Signal &&other) {
  if (this != &other) {
    // Clean up the current object's resources (like the destructor does)
    if (signum_ >= 0 && signum_ < 32 && notifier[signum_].is_init) {
      ::signal(signum_ + 1, SIG_DFL);
      notifier[signum_].is_init = false;
    }

    // Move the other object's state to this object
    signum_ = other.signum_;
    other.signum_ = -1;
  }
  return *this;
}

/**
 * Sends the signal to the current process using the raise system call[cite:
 * 85].
 */
bool Signal::raise() const {
  // Return false if in invalid state
  if (signum_ < 0 || signum_ >= 32) {
    return false;
  }
  return ::raise(signum()) == 0;
}

/**
 * Sends the signal to a specific process using the kill system call[cite: 85].
 */
bool Signal::kill(pid_t pid) const {
  // Return false if in invalid state
  if (signum_ < 0 || signum_ >= 32) {
    return false;
  }
  return ::kill(pid, signum()) == 0;
}

int Signal::signum() const {
  // Return negative value if in invalid state
  if (signum_ < 0) {
    return -1;
  }
  return signum_ + 1;
}

/**
 * Waits for the signal to arrive by checking the read-end of the pipe[cite:
 * 79].
 */
bool Signal::wait(const rix::util::Duration &d) const {
  if (signum_ < 0 || signum_ >= 32 || !notifier[signum_].is_init)
    return false;

  // First, check if the pipe is readable within the given duration
  if (notifier[signum_].pipe[0].wait_for_readable(d)) {
    // If readable, consume one byte from the pipe to "consume" the notification
    uint8_t val;
    notifier[signum_].pipe[0].read(&val, 1);
    return true;
  }

  return false;
}

/**
 * The async-signal-safe handler that writes to the pipe[cite: 75, 76].
 */
void Signal::handler(int signum) {
  int idx = signum - 1;
  if (idx >= 0 && idx < 32 && notifier[idx].is_init) {
    uint8_t val = 1;
    // Write to the write-end of the pipe (index 1) [cite: 72, 75]
    notifier[idx].pipe[1].write(&val, 1);
  }
}

} // namespace ipc
} // namespace rix