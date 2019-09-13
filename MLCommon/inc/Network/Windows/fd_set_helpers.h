#ifndef FD_SET_HELPERS_H_BAC76
#define FD_SET_HELPERS_H_BAC76

#include <winsock2.h>
#include <ws2tcpip.h>

#include <chrono>
#include <functional>
#include <vector>

class SelectWrapper;
class FdSet {
 public:
  using Consumer = std::function<void(SOCKET)>;
  using Builder = std::function<void(const Consumer&)>;
  using SocketStreamNamePair = std::pair<SOCKET, std::string>;
  using SocketVector = std::vector<SocketStreamNamePair>;

  FdSet(const SocketVector& vect) {
    FD_ZERO(&set);
    for (const auto& pair : vect) {
      addSocket(pair.first);
    }
  }

  FdSet(const Builder& builder) {
    FD_ZERO(&set);
    builder([=](SOCKET socket) { addSocket(socket); });
  }

 private:
  FD_SET set;
  void addSocket(SOCKET socket) { FD_SET(socket, &set); }
  friend class SelectWrapper;
};

class SelectWrapper {
 public:
  using Matcher = std::function<bool(SOCKET)>;
  using Consumer = std::function<void(bool isRead, const Matcher&)>;
  using OnError = std::function<void(int errorCode)>;
  SelectWrapper(FdSet* readSet, FdSet* writeSet,
                const std::chrono::milliseconds& waitTime)
      : rSet(readSet != nullptr ? &(readSet->set) : nullptr),
        wSet(writeSet != nullptr ? &(writeSet->set) : nullptr) {
    using namespace std::chrono;
    seconds const sec = duration_cast<std::chrono::seconds>(waitTime);

    timeout.tv_sec = static_cast<long>(sec.count());
    timeout.tv_usec =
        static_cast<long>(duration_cast<microseconds>(waitTime - sec).count());
  }

  void operator()(const Consumer& consumer, const OnError& onError) {
    int count = select(0, rSet, wSet, nullptr, &timeout);
    if (count > 0) {
      if (rSet != nullptr) {
        consumer(true, [&](SOCKET socket) { return FD_ISSET(socket, rSet); });
      }
      if (wSet != nullptr) {
        consumer(false, [&](SOCKET socket) { return FD_ISSET(socket, wSet); });
      }
    } else if (count < 0 and onError) {
      onError(WSAGetLastError());
    }
  }

 private:
  PFD_SET rSet;
  PFD_SET wSet;
  TIMEVAL timeout;
};

#endif  //! FD_SET_HELPERS_H_BAC76
