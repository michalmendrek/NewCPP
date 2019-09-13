#include "Network/Ipc/IpcTransmitter.h"

#include <plog/Log.h>
#include <chrono>

namespace ml {

IpcTransmitter::IpcTransmitter(const std::vector<SharedSocket>& sockets)
    : Worker("IpcTransmitter"), sockets(sockets) {}

void IpcTransmitter::sendData(const std::string& socketUri, SharedData data) {
  std::lock_guard<std::mutex> guard(dataMutex);

  auto it =
      std::find_if(sockets.cbegin(), sockets.cend(),
                   [&socketUri](const std::shared_ptr<IIpcSocket>& socket) {
                     return socket->getSocketUri() == socketUri;
                   });

  if (it != sockets.end()) {
    listOfDataToSend.push_back(std::make_pair(*it, data));
    readyToSend.notify_one();
  }
}

IpcTransmitter::DataToSend IpcTransmitter::getHead() {
  std::lock_guard<std::mutex> guard(dataMutex);

  if (listOfDataToSend.empty()) {
    DataToSend dataPack;
    dataPack.empty = true;
    return dataPack;
  }

  const auto iter = listOfDataToSend.begin();
  DataToSend result{false, iter->first, iter->second};
  listOfDataToSend.erase(listOfDataToSend.begin());
  return result;
}

void IpcTransmitter::sendToClients() {
  while (true) {
    DataToSend dataPack = getHead();
    if (dataPack.empty) {
      break;
    }
    innerSendToClients(dataPack);
  }
}

void IpcTransmitter::waitForData() {
  std::unique_lock<std::mutex> lock(dataMutex);
  auto now = std::chrono::system_clock::now();
  readyToSend.wait_until(lock, now + std::chrono::milliseconds(50), [this] {
    return (not listOfDataToSend.empty()) || StopRequested();
  });
}

void IpcTransmitter::Loop() {
  waitForData();
  sendToClients();
}

}  // namespace ml
