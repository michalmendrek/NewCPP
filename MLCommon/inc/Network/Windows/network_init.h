#ifndef NETWORK_INIT_H_32874
#define NETWORK_INIT_H_32874

#include <plog/Log.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include <memory>
#include <mutex>
#include <thread>
namespace ml {

class NetworkSetup {
 public:
  static std::shared_ptr<NetworkSetup> initNetwork() {
    std::lock_guard<std::mutex> guard(mutex);
    if (not instance.expired()) {
      return instance.lock();
    }
    std::shared_ptr<NetworkSetup> shared =
        std::shared_ptr<NetworkSetup>(new NetworkSetup());
    instance = shared;
    return shared;
  }

  ~NetworkSetup() {
    LOGI << "Cleanup of network layer.";
    WSACleanup();
  }

 private:
  static std::mutex mutex;
  static std::weak_ptr<NetworkSetup> instance;

  NetworkSetup() {
    LOGI << "Startup of network layer.";
    WSADATA wsaData{};
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
      LOGE << "Can't init network, error:" << result;
      throw std::runtime_error("Can't init network layer, this is critical.");
    }
  }
};
}  // namespace ml
#endif  // ! NETWORK_INIT_H_32874
