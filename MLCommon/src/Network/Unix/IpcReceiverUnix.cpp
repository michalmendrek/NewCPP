#include <sys/socket.h>
#include "EPoolWorker.h"
#include "IpcSocketUnix.h"
#include "Network/Ipc/IpcReceiver.h"
#include "Types/DataContainer.h"

namespace ml {
class IpcReceiverUnix : public IpcReceiver::PrivateImpl {
 public:
  IpcReceiverUnix() { epollWorker.prepareEPoll(); }
  virtual ~IpcReceiverUnix() override;
  virtual void loop(const IpcReceiver::SocketVector& sockets,
                    const IpcReceiver::Buffer& buffer,
                    IIpcListener& listener) override {
    // Note: ValidateEPoll & PurgeSockets is very naive but easy implementation,
    // upgrade if needed
    ValidateEPoll(sockets);
    std::vector<int> dropDescriptors;

    epollWorker.waitEPoll(
        [&](const epoll_event& event) {
          if (event.events & EPOLLIN) {
            ssize_t readBytes =
                read(event.data.fd, const_cast<char*>(buffer.data()),
                     buffer.capacity());
            if (readBytes > 0) {
              const auto it = std::find_if(
                  managedDescriptors.cbegin(), managedDescriptors.cend(),
                  [&](const SocketDescriptorIpcSocketPair& pair) {
                    return pair.first == event.data.fd;
                  });
              if (it == managedDescriptors.cend()) {
                LOGE << "There is no such file descriptor in managed "
                        "descriptors";
              } else {
                const DataContainer readData{const_cast<char*>(buffer.data()),
                                             static_cast<size_t>(readBytes)};
                listener.dispatchData(
                    readData, it->second,
                    event.data.fd);  // NOTE: This must be sync call!
              }
            }
          }
          if ((event.events & EPOLLHUP) or (event.events & EPOLLRDHUP)) {
            // close socket
            dropDescriptors.push_back(event.data.fd);
          }
        },

        [](const int errorCode) {
          LOGE << "Error on epoll read, code:" << errorCode;
        });

    if (not dropDescriptors.empty()) {
      purgeSockets(sockets, dropDescriptors);
    }
  }

 private:
  static constexpr int EPOOL_EVENTS = 10;
  static constexpr int EPOOL_WAIT = 100;
  using EPWorker = EPollWorker<EPOOL_EVENTS, EPOOL_WAIT>;
  using SocketDescriptorIpcSocketPair = std::pair<int, std::string>;
  using DescriptorsCollection = std::set<SocketDescriptorIpcSocketPair>;
  DescriptorsCollection managedDescriptors;
  EPWorker epollWorker;

  IpcSocketUnix* findParent(const IpcReceiver::SocketVector& sockets,
                            const int socketDescriptor) {
    auto iter = std::find_if(
        sockets.cbegin(), sockets.cend(),
        [&](const std::shared_ptr<IIpcSocket>& baseSocket) {
          IpcSocketUnix* socket = static_cast<IpcSocketUnix*>(baseSocket.get());
          const auto tmp = socket->getCopyOfClientsDescriptor();
          const auto it = std::find(tmp.cbegin(), tmp.cend(), socketDescriptor);
          return it != tmp.cend();
        });
    return (iter == sockets.cend()) ? nullptr
                                    : static_cast<IpcSocketUnix*>(iter->get());
  }

  void purgeSockets(const IpcReceiver::SocketVector& sockets,
                    const std::vector<int>& socketsDescriptors) {
    for (const int clientDescr : socketsDescriptors) {
      IpcSocketUnix* parent = findParent(sockets, clientDescr);
      if (parent) {
        parent->removeClientDescriptor(clientDescr);
      }
    }
  }

  const DescriptorsCollection collectDescriptors(
      const IpcReceiver::SocketVector& sockets) {
    DescriptorsCollection result;
    for (const std::shared_ptr<IIpcSocket>& s : sockets) {
      IpcSocketUnix* socket = static_cast<IpcSocketUnix*>(s.get());
      const auto tmp = socket->getCopyOfClientsDescriptor();
      std::for_each(std::begin(tmp), std::end(tmp), [&](const int& fd) {
        result.insert(std::make_pair(fd, s->getSocketName()));
      });
    }
    return result;
  }

  void ValidateEPoll(const IpcReceiver::SocketVector& sockets) {
    const DescriptorsCollection& allCurrent = collectDescriptors(sockets);

    // remove deleted decriptors
    std::vector<SocketDescriptorIpcSocketPair> tmp;
    std::set_difference(managedDescriptors.begin(), managedDescriptors.end(),
                        allCurrent.begin(), allCurrent.end(),
                        std::inserter(tmp, tmp.begin()));
    for (const SocketDescriptorIpcSocketPair& pair : tmp) {
      epollWorker.removeFromEPoll(pair.first);
      LOGD << "Removing socket descriptor " << pair.first;
    }

    // add new descriptors
    tmp.clear();
    std::set_difference(allCurrent.begin(), allCurrent.end(),
                        managedDescriptors.begin(), managedDescriptors.end(),
                        std::inserter(tmp, tmp.begin()));
    for (const SocketDescriptorIpcSocketPair& pair : tmp) {
      epollWorker.addToEPoll(pair.first,
                             EPOLLRDHUP | EPOLLHUP | EPOLLIN | EPOLLET);
      LOGD << "Adding socket descriptor " << pair.first;
    }

    managedDescriptors = allCurrent;
  }
};

IpcReceiverUnix::~IpcReceiverUnix() {}
std::unique_ptr<IpcReceiver::PrivateImpl> IpcReceiver::providePrivateImpl() {
  return std::unique_ptr<IpcReceiver::PrivateImpl>(new IpcReceiverUnix());
}
}  // namespace ml
