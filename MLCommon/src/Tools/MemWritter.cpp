#include "MemWritter.h"

int ml::MemWritter::Write(const void* data, size_t count) {
  size_t amount = std::min(count, size - position);
  if (amount == 0) {
    return -1;
  }

  const uint8_t* ptr{reinterpret_cast<const uint8_t*>(data)};
  std::copy(ptr, ptr + amount, bufferPtr + position);
  position += amount;

  return static_cast<int>(amount);
}
