#ifndef IPC_RECEIVER_H
#define IPC_RECEIVER_H

#include <Network/Ipc/IIpcListener.h>
#include <Network/Ipc/IIpcSocket.h>
#include <TimerFramework/Worker.h>
#include <Types/DataContainer.h>
#include <plog/Log.h>

#include <set>
#include <vector>

namespace ml {

class IpcReceiver : public Worker {
 public:
  using SocketVector = std::vector<std::shared_ptr<IIpcSocket>>;
  using Buffer = std::vector<char>;
  class PrivateImpl {
   public:
    virtual ~PrivateImpl();
    virtual void loop(const SocketVector& sockets, const Buffer& buffer,
                      IIpcListener& listener) = 0;
  };

  IpcReceiver(const SocketVector& sockets, IIpcListener& listener,
              const size_t sizeOfBuffer);

 private:
  const SocketVector& sockets;
  IIpcListener& listener;
  size_t sizeOfBuffer;
  std::vector<char> buffer;
  std::unique_ptr<PrivateImpl> privateImpl;

  virtual void Loop() override;
  std::unique_ptr<PrivateImpl> providePrivateImpl();
};

}  // namespace ml

#endif  // ! IPC_RECEIVER_H
