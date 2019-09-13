#include "Network/Ipc/IpcTransmitter.h"

#include <plog/Log.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "IpcSocketWindows.h"

namespace ml {

void IpcTransmitter::innerSendToClients(const DataToSend& dataPack) {
  IpcSocketWindows& ipcSock = static_cast<IpcSocketWindows&>(*dataPack.socket);
  std::vector<SOCKET> clientsDescriptor = ipcSock.getCopyOfClientsDescriptor();
  for (SOCKET clientDescriptor : clientsDescriptor) {
    int result = send(clientDescriptor, dataPack.data->GetDataPtr(),
                      static_cast<int>(dataPack.data->GetSize()), 0);
    if (result == SOCKET_ERROR) {
      LOGD << "Error or send, closing connection with descriptor "
           << clientDescriptor << " error code:" << WSAGetLastError();
      ipcSock.removeClientDescriptor(clientDescriptor);
      closesocket(clientDescriptor);
    }
  }
}
}  // namespace ml
