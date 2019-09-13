#ifndef IPC_TRANSMITTER_H
#define IPC_TRANSMITTER_H

#include <Network/Ipc/IIpcSocket.h>
#include <TimerFramework/Worker.h>
#include <Types/DataContainer.h>

#include <condition_variable>
#include <list>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

namespace ml {

class IpcTransmitter : public Worker {
 public:
  using SharedSocket = std::shared_ptr<IIpcSocket>;
  using SharedData = std::shared_ptr<DataContainer>;

  IpcTransmitter(const std::vector<SharedSocket>& sockets);
  void sendData(const std::string& socketAddr, SharedData data);

 protected:
  struct DataToSend {
    bool empty;
    SharedSocket socket;
    SharedData data;
  };
  void innerSendToClients(const DataToSend& dataPack);

 private:
  using SocketDataPair = std::pair<SharedSocket, SharedData>;

  const std::vector<SharedSocket>& sockets;
  std::list<SocketDataPair> listOfDataToSend;

  std::mutex dataMutex;
  std::condition_variable readyToSend;

  virtual void Loop() override;
  void sendToClients();
  DataToSend getHead();
  void waitForData();
};

}  // namespace ml

#endif  // ! IPC_TRANSMITTER_H
