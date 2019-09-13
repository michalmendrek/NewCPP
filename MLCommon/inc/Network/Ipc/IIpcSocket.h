#ifndef IIPC_SOCKET_H
#define IIPC_SOCKET_H

#include <algorithm>
#include <memory>
#include <mutex>
#include <string>
#include <vector>
namespace ml {

class IIpcSocket {
 public:
  virtual ~IIpcSocket();
  const std::string& getSocketUri() const { return socketUri; }
  const std::string& getSocketName() const { return socketName; }

  static std::shared_ptr<IIpcSocket> create(const std::string& uri,
                                            const std::string& name);

 protected:
  std::string socketUri;
  std::string socketName;
  IIpcSocket(const std::string& socketUri, const std::string& socketName)
      : socketUri(socketUri), socketName{socketName} {}
};

template <typename T>
class IpcGenericSocket {
 public:
  const T& getSocketDescriptor() const { return socketDescriptor; }

  void setSocketDescriptor(const T& socketDescriptor) {
    this->socketDescriptor = socketDescriptor;
  }

  void addClientDecriptor(const T& clientDescriptor) {
    std::lock_guard<std::mutex> guard(clientsMutex);
    auto it = std::find(clientsDescriptor.begin(), clientsDescriptor.end(),
                        clientDescriptor);
    if (it == clientsDescriptor.end()) {
      clientsDescriptor.push_back(clientDescriptor);
    }
  }

  void removeClientDescriptor(T clientDescriptor) {
    std::lock_guard<std::mutex> guard(clientsMutex);
    auto it = std::find(clientsDescriptor.begin(), clientsDescriptor.end(),
                        clientDescriptor);
    if (it != clientsDescriptor.end()) {
      clientsDescriptor.erase(it);
    }
  }

  void clearClientsDescriptor() {
    std::lock_guard<std::mutex> guard(clientsMutex);
    clientsDescriptor.clear();
  }

  std::vector<T> getCopyOfClientsDescriptor() {
    std::lock_guard<std::mutex> guard(clientsMutex);
    return clientsDescriptor;
  }

 protected:
  std::mutex clientsMutex;
  T socketDescriptor{};
  std::vector<T> clientsDescriptor;
};

}  // namespace ml

#endif  // ! IIPC_SOCKET_H
