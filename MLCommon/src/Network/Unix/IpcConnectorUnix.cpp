#include "Network/Ipc/IpcConnector.h"
#include <grp.h>
#include <pwd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "EPoolWorker.h"
#include "IpcSocketUnix.h"

namespace ml {

class IpcConnectorUnix : public IpcConnector::PrivateImpl {
 public:
  IpcConnectorUnix(IpcConnector& parent) : parent(parent) {}
  virtual ~IpcConnectorUnix() override;
  virtual void loop(IpcConnector::SocketVector& sockets) override {
    (void)sockets;
    epollWorker.waitEPoll(
        [](const epoll_event& event) {
          if (event.events & EPOLLIN) {
            IpcSocketUnix* socket = static_cast<IpcSocketUnix*>(event.data.ptr);

            int result =
                accept(socket->getSocketDescriptor(), nullptr, nullptr);
            if (result >= 0) {
              LOGD << "Accept new connection on " << socket->getSocketUri()
                   << ", used descriptor:" << result;
              socket->addClientDecriptor(result);
            } else {
              LOGE << "Accept socket error on " << socket->getSocketUri();
            }
          }
        },

        [](const int errorCode) {
          LOGE << "Error on epoll accept connection, error code:" << errorCode;
        });
  }

  virtual bool bindSocket(IIpcSocket& ipcSocketBase) override {
    IpcSocketUnix& socket = static_cast<IpcSocketUnix&>(ipcSocketBase);

    if (socket.socketPath.empty()) {
      return false;
    }
    LOGI << "Binding " << socket.socketPath;
    int socketDescriptor{::socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0)};
    if (socketDescriptor == -1) {
      LOGE << "Socket error";
      return false;
    }
    struct sockaddr_un address;
    socket.setSocketDescriptor(socketDescriptor);

    memset(&address, 0, sizeof(address));
    address.sun_family = AF_UNIX;
    size_t addressLength =
        std::min(socket.socketPath.length(), sizeof(address.sun_path) - 1);
    strncpy(address.sun_path, socket.socketPath.data(), addressLength);

    unlink(socket.socketPath.data());

    if (bind(socket.getSocketDescriptor(),
             reinterpret_cast<struct sockaddr*>(&address),
             sizeof(address)) == -1) {
      LOGE << "Socker bind error for " << socket.socketPath
           << ", error:" << errno;
      return false;
    }

    constexpr int numberOfQueuedRequestsBeforeRefuse = 5;

    if (listen(socket.getSocketDescriptor(),
               numberOfQueuedRequestsBeforeRefuse) == -1) {
      LOGE << "listen error for " << socket.socketPath;
      return false;
    }

    applyPermissions(socket.socketPath);
    return true;
  }

  virtual void stop(IpcConnector::SocketVector& sockets) override {
    (void)sockets;
    epollWorker.cleanupEPoll();
  }

  virtual void start(IpcConnector::SocketVector& sockets) override {
    if (not epollWorker.prepareEPoll(sockets.size(), [&](const size_t index) {
          IIpcSocket* ipcSocketBase = sockets[index].get();
          IpcSocketUnix* socket = static_cast<IpcSocketUnix*>(ipcSocketBase);
          return EPWorker::EpollItem{EPOLLIN | EPOLLRDHUP | EPOLLET,
                                     socket->getSocketDescriptor(), socket};
        })) {
      epollWorker.cleanupEPoll();
      throw std::logic_error("Can't create epoll");
    }
  }

  virtual void closeSocket(IIpcSocket& ipcSocketBase) override {
    IpcSocketUnix& ipcSocket = static_cast<IpcSocketUnix&>(ipcSocketBase);
    closeSocket(ipcSocket);
  }

 private:
  static constexpr int EPOOL_EVENTS = 10;
  static constexpr int EPOOL_WAIT = 1 * 1000;
  using EPWorker = EPollWorker<EPOOL_EVENTS, EPOOL_WAIT>;
  EPWorker epollWorker;
  IpcConnector& parent;

  void applyPermissions(const std::string& address) const {
    static const int RwxOwnerAndGroup = 0770;
    if (chmod(address.data(), RwxOwnerAndGroup) == -1) {
      LOGE << address << ":Failed to change mod, error:" << errno;
      return;
    }
    const auto uid = getuid();
    const auto grp = getgrnam(parent.getUserGroup().c_str());
    if (grp == nullptr) {
      LOGE << address << ":Failed to get gid for: " << parent.getUserGroup();
      return;
    }
    const auto gid = grp->gr_gid;
    if (chown(address.data(), uid, gid) == -1) {
      LOGE << address << ":Failed to change owner, error:" << errno;
      return;
    }
  }

  void closeSocket(IpcSocketUnix& ipcSocket) {
    close(ipcSocket.getSocketDescriptor());
    shutdown(ipcSocket.getSocketDescriptor(), SHUT_RDWR);
    ipcSocket.setSocketDescriptor(-1);

    for (int clientDescriptor : ipcSocket.getCopyOfClientsDescriptor()) {
      close(clientDescriptor);
      shutdown(clientDescriptor, SHUT_RDWR);
    }
    ipcSocket.clearClientsDescriptor();
  }
};

IpcConnectorUnix::~IpcConnectorUnix() {}

std::unique_ptr<IpcConnector::PrivateImpl> IpcConnector::providePrivateImpl() {
  return std::unique_ptr<IpcConnector::PrivateImpl>{
      new IpcConnectorUnix(*this)};
}
}  // namespace - ml
