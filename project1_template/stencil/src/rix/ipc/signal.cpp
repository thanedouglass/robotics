#include "rix/ipc/signal.hpp"
#include <csignal>
#include <unistd.h>

namespace rix {
namespace ipc {

std::array<Signal::SignalNotifier, 32> Signal::notifier = {};

/**
 * Initializes the signal handler and the notification pipe.
 */
Signal::Signal(int signum) : signum_(signum - 1) {
  if (signum_ < 0 || signum_ >= 32)
    return;

  // Initialize the notifier if it hasn't been yet [cite: 73]
  if (!notifier[signum_].is_init) {
    notifier[signum_].pipe = Pipe::create(); // Create 2 Pipes (read/write)
    notifier[signum_].is_init = true;

    // Register the static handler for this signal [cite: 74]
    ::signal(signum, Signal::handler);
  }
}

Signal::~Signal() {
  // Note: Usually, we don't reset SIG_DFL here because other Signal objects
  // might still be watching this specific signum in the same process.
}

Signal::Signal(Signal &&other) : signum_(-1) {
  std::swap(signum_, other.signum_);
}

Signal &Signal::operator=(Signal &&other) {
  if (this != &other) {
    signum_ = other.signum_;
    other.signum_ = -1;
  }
  return *this;
}

/**
 * Sends the signal to the current process using the raise system call[cite:
 * 85].
 */
bool Signal::raise() const { return ::raise(signum()) == 0; }

/**
 * Sends the signal to a specific process using the kill system call[cite: 85].
 */
bool Signal::kill(pid_t pid) const { return ::kill(pid, signum()) == 0; }

int Signal::signum() const { return signum_ + 1; }

/**
 * Waits for the signal to arrive by checking the read-end of the pipe[cite:
 * 79].
 */
bool Signal::wait(const rix::util::Duration &d) const {
  if (signum_ < 0 || signum_ >= 32 || !notifier[signum_].is_init)
    return false;

  // The read end of the pipe is index 0
  if (notifier[signum_].pipe[0].wait_for_readable(d)) {
    // Consume ALL bytes written by the handler to reset the "readable" state
    // Multiple signals may have been raised, writing multiple bytes
    uint8_t buf[256];
    while (notifier[signum_].pipe[0].read(buf, sizeof(buf)) > 0) {
      // Keep reading until pipe is empty
    }
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