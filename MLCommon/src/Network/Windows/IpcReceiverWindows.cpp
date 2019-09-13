#include "IpcSocketWindows.h"
#include "Network/Ipc/IpcReceiver.h"
#include "Types/DataContainer.h"
#include "fd_set_helpers.h"

namespace ml {
class IpcReceiverWindows : public IpcReceiver::PrivateImpl {
 public:
  virtual ~IpcReceiverWindows() override;
  virtual void loop(const IpcReceiver::SocketVector& sockets,
                    const IpcReceiver::Buffer& buffer,
                    IIpcListener& listener) override {
    collectSockets(sockets);
    if (allSockets.empty()) {
      return;
    }
    FdSet readSet{allSockets};
    SelectWrapper wrapper{&readSet, nullptr, SELECT_TIMEOUT};
    wrapper(
        [&](bool isRead, const SelectWrapper::Matcher& matcher) {
          (void)isRead;
          auto it =
              std::partition(allSockets.begin(), allSockets.end(),
                             [matcher](const FdSet::SocketStreamNamePair &pair){ return matcher(pair.first); });
          if (it != allSockets.end()) {
            allSockets.erase(it, allSockets.end());
          }
          if (not allSockets.empty()) {
            readFromSockets(buffer, listener);
          }
        },
        [](int error) {
          LOGW << "Error while reading from sockets: " << error;
        });
  }

 private:
  static const std::chrono::milliseconds SELECT_TIMEOUT;
  FdSet::SocketVector allSockets;

  void collectSockets(const IpcReceiver::SocketVector& sockets) {
    allSockets.clear();
    for (auto& socket : sockets) {
      IpcSocketWindows& ipcSocket = static_cast<IpcSocketWindows&>(*socket);
      const auto& descr = ipcSocket.getCopyOfClientsDescriptor();
      std::for_each(std::begin(descr), std::end(descr), [&](const SOCKET& fd) {
        allSockets.insert(allSockets.end(), std::make_pair(fd, socket->getSocketName()));
      });
    }
  }

  void readFromSockets(const IpcReceiver::Buffer& buffer,
                       IIpcListener& listener) {
    for (const auto& pair : allSockets) {
      ssize_t readBytes = recv(pair.first, const_cast<char*>(buffer.data()),
                               static_cast<int>(buffer.capacity()), 0);
      if (readBytes > 0) {
        const DataContainer readData{const_cast<char*>(buffer.data()),
                                     static_cast<size_t>(readBytes)};
        listener.dispatchData(readData, pair.second, static_cast<int>(pair.first));  // NOTE: This must be sync call!
      }
    }
  }
};

std::unique_ptr<IpcReceiver::PrivateImpl> IpcReceiver::providePrivateImpl() {
  return std::unique_ptr<IpcReceiver::PrivateImpl>(new IpcReceiverWindows());
}

IpcReceiverWindows::~IpcReceiverWindows() {}

const std::chrono::milliseconds IpcReceiverWindows::SELECT_TIMEOUT{500};
}  // namespace ml
