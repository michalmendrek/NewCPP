#ifndef IPC_TEST_H
#define IPC_TEST_H

#include <memory>
#include "Network/Ipc/IIpcListener.h"
#include "Network/Ipc/IIpcSocket.h"
#include "Network/Ipc/IpcConnector.h"
#include "Network/Ipc/IpcReceiver.h"
#include "Network/Ipc/IpcTransmitter.h"

class IpcTest : public ml::IIpcListener {
 public:
  using ShrSocket = std::shared_ptr<ml::IIpcSocket>;

  IpcTest(const std::vector<ShrSocket>& sockets);
  void start();
  void stop();
  void dispatchData(const ml::DataContainer& data,
                    const std::string& socketName,
                    const int descriptor) override;

 private:
  std::vector<ShrSocket> sockets;
  ml::IpcReceiver receiver;
  ml::IpcConnector con;
  ml::IpcTransmitter transmiter;
};

#endif  // IPC_TEST_H
