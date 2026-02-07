#pragma once

#include <array>
#include <cstdint>
#include <cstring>
#include <string>
#include <type_traits>
#include <vector>

#include "rix/msg/message.hpp"

namespace rix {
namespace msg {
namespace detail {

// --- Serialization Implementations ---

template <typename T> inline size_t size_number(const T &val) {
  return sizeof(T);
}

template <typename T>
inline void serialize_number(uint8_t *dst, size_t &offset, const T &src) {
  static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type");
  std::memcpy(dst + offset, &src, sizeof(T));
  offset += sizeof(T);
}

inline void serialize_string(uint8_t *dst, size_t &offset,
                             const std::string &src) {
  uint32_t len = static_cast<uint32_t>(src.size());
  serialize_number(dst, offset, len); // Prefix with length
  std::memcpy(dst + offset, src.data(), len);
  offset += len;
}

inline void serialize_message(uint8_t *dst, size_t &offset,
                              const Message &src) {
  src.serialize(dst, offset);
}

template <typename T, size_t N>
inline void serialize_number_array(uint8_t *dst, size_t &offset,
                                   const std::array<T, N> &src) {
  for (const auto &val : src)
    serialize_number(dst, offset, val);
}

template <size_t N>
inline void serialize_string_array(uint8_t *dst, size_t &offset,
                                   const std::array<std::string, N> &src) {
  for (const auto &s : src)
    serialize_string(dst, offset, s);
}

template <typename T, size_t N>
inline void serialize_message_array(uint8_t *dst, size_t &offset,
                                    const std::array<T, N> &src) {
  for (const auto &m : src)
    serialize_message(dst, offset, m);
}

template <typename T>
inline void serialize_number_vector(uint8_t *dst, size_t &offset,
                                    const std::vector<T> &src) {
  uint32_t size = static_cast<uint32_t>(src.size());
  serialize_number(dst, offset, size); // Prefix with element count
  for (const auto &val : src)
    serialize_number(dst, offset, val);
}

inline void serialize_string_vector(uint8_t *dst, size_t &offset,
                                    const std::vector<std::string> &src) {
  uint32_t size = static_cast<uint32_t>(src.size());
  serialize_number(dst, offset, size);
  for (const auto &s : src)
    serialize_string(dst, offset, s);
}

template <typename T>
inline void serialize_message_vector(uint8_t *dst, size_t &offset,
                                     const std::vector<T> &src) {
  uint32_t size = static_cast<uint32_t>(src.size());
  serialize_number(dst, offset, size);
  for (const auto &m : src)
    serialize_message(dst, offset, m);
}

// --- Deserialization Implementations ---

template <typename T>
inline bool deserialize_number(T &dst, const uint8_t *src, size_t size,
                               size_t &offset) {
  if (offset + sizeof(T) > size)
    return false;
  std::memcpy(&dst, src + offset, sizeof(T));
  offset += sizeof(T);
  return true;
}

inline bool deserialize_string(std::string &dst, const uint8_t *src,
                               size_t size, size_t &offset) {
  uint32_t len;
  if (!deserialize_number(len, src, size, offset))
    return false;
  if (offset + len > size)
    return false;
  dst.assign(reinterpret_cast<const char *>(src + offset), len);
  offset += len;
  return true;
}

inline bool deserialize_message(Message &dst, const uint8_t *src, size_t size,
                                size_t &offset) {
  return dst.deserialize(src, size, offset);
}

template <typename T, size_t N>
inline bool deserialize_number_array(std::array<T, N> &dst, const uint8_t *src,
                                     size_t size, size_t &offset) {
  for (size_t i = 0; i < N; ++i) {
    if (!deserialize_number(dst[i], src, size, offset))
      return false;
  }
  return true;
}

template <size_t N>
inline bool deserialize_string_array(std::array<std::string, N> &dst,
                                     const uint8_t *src, size_t size,
                                     size_t &offset) {
  for (size_t i = 0; i < N; ++i) {
    if (!deserialize_string(dst[i], src, size, offset))
      return false;
  }
  return true;
}

template <typename T, size_t N>
inline bool deserialize_message_array(std::array<T, N> &dst, const uint8_t *src,
                                      size_t size, size_t &offset) {
  for (size_t i = 0; i < N; ++i) {
    if (!deserialize_message(dst[i], src, size, offset))
      return false;
  }
  return true;
}

template <typename T>
inline bool deserialize_number_vector(std::vector<T> &dst, const uint8_t *src,
                                      size_t size, size_t &offset) {
  uint32_t count;
  if (!deserialize_number(count, src, size, offset))
    return false;
  dst.resize(count);
  for (uint32_t i = 0; i < count; ++i) {
    if (!deserialize_number(dst[i], src, size, offset))
      return false;
  }
  return true;
}

inline bool deserialize_string_vector(std::vector<std::string> &dst,
                                      const uint8_t *src, size_t size,
                                      size_t &offset) {
  uint32_t count;
  if (!deserialize_number(count, src, size, offset))
    return false;
  dst.resize(count);
  for (uint32_t i = 0; i < count; ++i) {
    if (!deserialize_string(dst[i], src, size, offset))
      return false;
  }
  return true;
}

template <typename T>
inline bool deserialize_message_vector(std::vector<T> &dst, const uint8_t *src,
                                       size_t size, size_t &offset) {
  uint32_t count;
  if (!deserialize_number(count, src, size, offset))
    return false;
  dst.resize(count);
  for (uint32_t i = 0; i < count; ++i) {
    if (!deserialize_message(dst[i], src, size, offset))
      return false;
  }
  return true;
}

} // namespace detail

// Public API wrappers in rix::msg namespace
template <typename T> inline size_t size_number(const T &val) {
  return detail::size_number(val);
}

template <typename T>
inline void serialize_number(uint8_t *dst, size_t &offset, const T &src) {
  detail::serialize_number(dst, offset, src);
}

template <typename T>
inline bool deserialize_number(T &dst, const uint8_t *src, size_t size,
                               size_t &offset) {
  return detail::deserialize_number(dst, src, size, offset);
}

inline void serialize_string(uint8_t *dst, size_t &offset,
                             const std::string &src) {
  detail::serialize_string(dst, offset, src);
}

inline bool deserialize_string(std::string &dst, const uint8_t *src,
                               size_t size, size_t &offset) {
  return detail::deserialize_string(dst, src, size, offset);
}

} // namespace msg
} // namespace rix