#include "network_init.h"

#include <mutex>
#include <thread>

namespace ml {
std::mutex NetworkSetup::mutex;
std::weak_ptr<NetworkSetup> NetworkSetup::instance;
}  // namespace ml
