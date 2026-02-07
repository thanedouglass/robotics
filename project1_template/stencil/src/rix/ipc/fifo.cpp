#include "rix/ipc/fifo.hpp"
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

namespace rix {
namespace ipc {

/**
 * Creates a FIFO using mkfifo and opens it with the specified mode.
 * Handles the nonblocking flag during the open() system call.
 */
Fifo::Fifo(const std::string &pathname, Mode mode, bool nonblocking)
    : pathname_(pathname), mode_(mode) {

  // Create the FIFO with read/write permissions for the user (0666)
  // If it already exists, mkfifo returns -1; we proceed to open it regardless.
  ::mkfifo(pathname_.c_str(), 0666);

  int flags;
  if (mode == Mode::READ) {
    flags = O_RDONLY;
  } else {
    flags = O_WRONLY;
  }

  if (nonblocking) {
    flags |= O_NONBLOCK;
  }

  // Use the open() system call to get the file descriptor
  fd_ = ::open(pathname_.c_str(), flags);
}

Fifo::Fifo() : File(), pathname_(""), mode_(Mode::READ) {}

/**
 * Copy constructor using File's dup logic to share the same file description.
 */
Fifo::Fifo(const Fifo &other)
    : File(other), pathname_(other.pathname_), mode_(other.mode_) {}

/**
 * Copy assignment operator using File's operator=.
 */
Fifo &Fifo::operator=(const Fifo &other) {
  if (this != &other) {
    File::operator=(other);
    pathname_ = other.pathname_;
    mode_ = other.mode_;
  }
  return *this;
}

Fifo::Fifo(Fifo &&other)
    : File(std::move(other)), pathname_(std::move(other.pathname_)),
      mode_(std::move(other.mode_)) {}

Fifo &Fifo::operator=(Fifo &&other) {
  if (this != &other) {
    File::operator=(std::move(other));
    pathname_ = std::move(other.pathname_);
    mode_ = std::move(other.mode_);
  }
  return *this;
}

Fifo::~Fifo() {
  // File destructor handles close(fd_).
}

std::string Fifo::pathname() const { return pathname_; }

Fifo::Mode Fifo::mode() const { return mode_; }

} // namespace ipc
} // namespace rix