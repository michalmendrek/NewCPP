#include <winsock2.h>
#include <ws2tcpip.h>

#include <chrono>
#include <memory>

#include "IpcSocketWindows.h"
#include "Network/Ipc/IpcConnector.h"
#include "fd_set_helpers.h"
#include "network_init.h"
namespace ml {

class IpcConnectorWindows : public IpcConnector::PrivateImpl {
 public:
  IpcConnectorWindows() : net(NetworkSetup::initNetwork()) {}

  virtual ~IpcConnectorWindows() override;

  virtual bool bindSocket(IIpcSocket& ipcSocketBase) override {
    IpcSocketWindows& ipcSocket =
        static_cast<IpcSocketWindows&>(ipcSocketBase);
    if (ipcSocket.host.empty() or ipcSocket.port == 0) {
      return false;
    }
    ipcSocket.setSocketDescriptor(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
    if (ipcSocket.getSocketDescriptor() == INVALID_SOCKET) {
      LOGE << "Socket failed with error: " << WSAGetLastError();
      return false;
    }

    hostent* thisHost = gethostbyname(ipcSocket.host.c_str());
    if (thisHost == nullptr) {
      LOGE << "gethostbyname failed with error: " << WSAGetLastError();
      closeSocket(ipcSocket);
      return false;
    }

    const char* ip =
        inet_ntoa(*reinterpret_cast<struct in_addr*>(*thisHost->h_addr_list));

    sockaddr_in service{};
    service.sin_family = AF_INET;
    service.sin_port = htons(ipcSocket.port);
    service.sin_addr.s_addr = inet_addr(ip);

    int result = bind(ipcSocket.getSocketDescriptor(),
                      reinterpret_cast<SOCKADDR*>(&service), sizeof(SOCKADDR));
    if (result != 0) {
      LOGE << "Bind failed with error: " << WSAGetLastError();
      closeSocket(ipcSocket);
      return false;
    }

    result = listen(ipcSocket.getSocketDescriptor(), 1);
    if (result != 0) {
      LOGE << "Listen failed with error:" << WSAGetLastError();
      closeSocket(ipcSocket);
      return false;
    }

    if (setSocketToNonBlock(ipcSocket.getSocketDescriptor()) == false) {
      closeSocket(ipcSocket);
      return false;
    }

    return true;
  }

  virtual void closeSocket(IIpcSocket& ipcSocketBase) override {
    IpcSocketWindows& ipcSocket =
        static_cast<IpcSocketWindows&>(ipcSocketBase);
    closeSocket(ipcSocket);
  }

  virtual void stop(IpcConnector::SocketVector& sockets) override {
    (void)sockets;
  }

  virtual void start(IpcConnector::SocketVector& sockets) override {
    (void)sockets;
  }

  virtual void loop(IpcConnector::SocketVector& sockets) override {
    FdSet readSet{[&](const FdSet::Consumer& addSocket) {
      for (auto& socket : sockets) {
        IpcSocketWindows& ipcSocket = static_cast<IpcSocketWindows&>(*socket);
        addSocket(ipcSocket.getSocketDescriptor());
      }
    }};

    SelectWrapper wrapper{&readSet, nullptr, SELECT_TIMEOUT};
    wrapper(
        [&](bool isRead, const SelectWrapper::Matcher& matcher) {
          (void)isRead;
          acceptIncommingSockets(matcher, sockets);
        },
        [](int error) {
          LOGW << "Error while accepting connections: " << error;
        });
  }

 private:
  static const std::chrono::milliseconds SELECT_TIMEOUT;

  std::shared_ptr<NetworkSetup> net;

  const std::string extractHost(const std::string& address) {
    auto pos = address.find(':');
    if (pos == std::string::npos) {
      return {};

    } else {
      return address.substr(0, pos);
    }
  }

  void acceptIncommingSockets(const SelectWrapper::Matcher& matcher,
                              IpcConnector::SocketVector& sockets) {
    for (auto& socket : sockets) {
      IpcSocketWindows& ipcSocket = static_cast<IpcSocketWindows&>(*socket);
      if (matcher(ipcSocket.getSocketDescriptor())) {
        SOCKET acceptSocket =
            accept(ipcSocket.getSocketDescriptor(), nullptr, nullptr);
        collectSocket(ipcSocket, acceptSocket);
      }
    }
  }

  void collectSocket(IpcSocketWindows& ipcSocket, SOCKET socket) {
    if (socket == INVALID_SOCKET) {
      if (WSAGetLastError() != WSAEWOULDBLOCK) {
        LOGE << "Accept socket error on " << ipcSocket.getSocketUri()
             << " code:" << WSAGetLastError();
      }
      return;
    }
    if (setSocketToNonBlock(socket)) {
      ipcSocket.addClientDecriptor(socket);
      LOGD << "Accept new connection on " << ipcSocket.getSocketUri();

    } else {
      LOGW << "Connection from " << ipcSocket.getSocketUri()
           << " is dropped due to error";
      closesocket(socket);
    }
  }

  bool setSocketToNonBlock(SOCKET socket) {
    unsigned long mode = 1;
    int result = ioctlsocket(socket, static_cast<long>(FIONBIO), &mode);
    if (result != 0) {
      LOGE << "Ioctlsocket failed with error:" << WSAGetLastError();
      return false;
    }
    return true;
  }

  void closeSocket(IpcSocketWindows& ipcSocket) {
    closesocket(ipcSocket.getSocketDescriptor());
    ipcSocket.setSocketDescriptor(INVALID_SOCKET);
    for (SOCKET clientDescriptor : ipcSocket.getCopyOfClientsDescriptor()) {
      closesocket(clientDescriptor);
    }
    ipcSocket.clearClientsDescriptor();
  }
};

std::unique_ptr<IpcConnector::PrivateImpl> IpcConnector::providePrivateImpl() {
  return std::unique_ptr<IpcConnector::PrivateImpl>{new IpcConnectorWindows()};
}

IpcConnectorWindows::~IpcConnectorWindows() {}

const std::chrono::milliseconds IpcConnectorWindows::SELECT_TIMEOUT{500};
}  // namespace ml
