#include "rix/ipc/pipe.hpp"
#include <unistd.h>
#include <utility>

namespace rix {
namespace ipc {

Pipe::Pipe() : File(), read_end_(false) {}

/**
 * @brief Factory method to create a pair of Pipe objects.
 */
std::array<Pipe, 2> Pipe::create() {
  int fds[2];
  if (::pipe(fds) != 0) {
    // In a real application, we might throw or return invalid objects.
    // For now, returning default objects (invalid fds) if pipe fails.
    return {};
  }
  return {Pipe(fds[0], true), Pipe(fds[1], false)};
}

/**
 * @brief Copy constructor.
 */
Pipe::Pipe(const Pipe &other) : File(other), read_end_(other.read_end_) {}

/**
 * @brief Copy assignment operator.
 */
Pipe &Pipe::operator=(const Pipe &other) {
  if (this != &other) {
    File::operator=(other);
    read_end_ = other.read_end_;
  }
  return *this;
}

Pipe::Pipe(Pipe &&other) : File(std::move(other)), read_end_(other.read_end_) {}

Pipe &Pipe::operator=(Pipe &&other) {
  if (this != &other) {
    // The base File move assignment already handles closing the current fd_
    File::operator=(std::move(other));
    read_end_ = other.read_end_;
  }
  return *this;
}

Pipe::~Pipe() {
  // File destructor handles closing the fd.
}

bool Pipe::is_read_end() const { return read_end_; }

bool Pipe::is_write_end() const { return !read_end_; }

Pipe::Pipe(int fd, bool read_end) : File(fd), read_end_(read_end) {}

} // namespace ipc
} // namespace rix