#ifndef IPC_CONNECTOR_H
#define IPC_CONNECTOR_H

#include <Network/Ipc/IIpcSocket.h>
#include <TimerFramework/Worker.h>
#include <plog/Log.h>

#include <memory>
#include <string>
#include <vector>

namespace ml {

class IpcConnector : public Worker {
 public:
  using SocketVector = std::vector<std::shared_ptr<IIpcSocket>>;
  class PrivateImpl {
   public:
    virtual ~PrivateImpl();
    virtual void start(SocketVector& sockets) = 0;
    virtual void stop(SocketVector& sockets) = 0;
    virtual bool bindSocket(IIpcSocket& ipcSocket) = 0;
    virtual void closeSocket(IIpcSocket& ipcSocketBase) = 0;
    virtual void loop(SocketVector& sockets) = 0;
  };
  IpcConnector(const SocketVector& sockets);

  void Start();
  void Stop();

  void setUserGroup(const std::string& group);
  const std::string& getUserGroup() const;

 protected:
  using Worker::Start;
  using Worker::Stop;

  std::unique_ptr<PrivateImpl> privateImpl;
  SocketVector sockets;
  bool isReady{false};
  std::string userGroup{"ml"};

  virtual void Loop() override;
  std::unique_ptr<PrivateImpl> providePrivateImpl();
};

}  // namespace ml

#endif  // ! IPC_CONNECTOR_H
