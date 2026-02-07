#include "rix/ipc/file.hpp"

namespace rix {
namespace ipc {

/**< TODO */
/**
 * @brief Removes the file specified by `pathname`.
 */
bool File::remove(const std::string &pathname) {
  return ::unlink(pathname.c_str()) == 0;
}

File::File() : fd_(-1) {}

File::File(int fd) : fd_(fd) {}

/**< TODO */
/**
 * @brief Creates a File object by opening the file specified by the path name.
 */
File::File(std::string pathname, int creation_flags, mode_t mode) {
  fd_ = ::open(pathname.c_str(), creation_flags, mode);
}

/**< TODO */
/**
 * @brief Copy constructor.
 */
File::File(const File &src) {
  if (src.fd_ >= 0) {
    fd_ = ::dup(src.fd_);
  } else {
    fd_ = -1;
  }
}

/**< TODO */
/**
 * @brief Assignment operator.
 */
File &File::operator=(const File &src) {
  if (this != &src) {
    if (fd_ >= 0) {
      ::close(fd_);
    }
    if (src.fd_ >= 0) {
      fd_ = ::dup(src.fd_);
    } else {
      fd_ = -1;
    }
  }
  return *this;
}

File::File(File &&src) : fd_(-1) { std::swap(fd_, src.fd_); }

File &File::operator=(File &&src) {
  if (fd_ >= 0) {
    ::close(fd_);
    fd_ = -1;
  }
  std::swap(fd_, src.fd_);
  return *this;
}

/**< TODO */
/**
 * @brief Destructor.
 */
File::~File() {
  if (fd_ >= 0) {
    ::close(fd_);
  }
}

/**< TODO */
/**
 * @brief Read `size` bytes from the file and store them in `dst`.
 */
ssize_t File::read(uint8_t *buffer, size_t size) const {
  return ::read(fd_, buffer, size);
}

/**< TODO */
/**
 * @brief Write `size` bytes from `src` to the file.
 */
ssize_t File::write(const uint8_t *buffer, size_t size) const {
  return ::write(fd_, buffer, size);
}

int File::fd() const { return fd_; }

/**< TODO */
/**
 * @brief Returns `true` if the file is in a valid state, `false` otherwise.
 */
bool File::ok() const { return fd_ >= 0; }

/**< TODO */
/**
 * @brief Toggles non-blocking IO operations.
 */
void File::set_nonblocking(bool status) {
  int flags = ::fcntl(fd_, F_GETFL);
  if (status) {
    flags |= O_NONBLOCK;
  } else {
    flags &= ~O_NONBLOCK;
  }
  ::fcntl(fd_, F_SETFL, flags);
}

/**< TODO */
/**
 * @brief Returns true if the file is in non-blocking mode.
 */
bool File::is_nonblocking() const {
  return (::fcntl(fd_, F_GETFL) & O_NONBLOCK) != 0;
}

/**< TODO */
/**
 * @brief Waits for the specified duration for the file to become writable.
 */
bool File::wait_for_writable(const util::Duration &duration) const {
  struct pollfd pfd;
  pfd.fd = fd_;
  pfd.events = POLLOUT;
  int timeout_ms =
      (duration == util::Duration::max()) ? -1 : duration.to_milliseconds();
  int ret = ::poll(&pfd, 1, timeout_ms);
  return ret > 0 && (pfd.revents & POLLOUT);
}

/**< TODO */
/**
 * @brief Waits for the specified duration for the file to become readable.
 */
bool File::wait_for_readable(const util::Duration &duration) const {
  struct pollfd pfd;
  pfd.fd = fd_;
  pfd.events = POLLIN;
  int timeout_ms =
      (duration == util::Duration::max()) ? -1 : duration.to_milliseconds();
  int ret = ::poll(&pfd, 1, timeout_ms);
  return ret > 0 && (pfd.revents & POLLIN);
}

} // namespace ipc
} // namespace rix