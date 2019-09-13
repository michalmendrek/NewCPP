#include "Network/Ipc/IpcTransmitter.h"
#include <plog/Log.h>
#include "IpcSocketUnix.h"

namespace ml {
void IpcTransmitter::innerSendToClients(const DataToSend& dataPack) {
  IpcSocketUnix& ipcSock = static_cast<IpcSocketUnix&>(*dataPack.socket);
  const auto clientsDescriptor = ipcSock.getCopyOfClientsDescriptor();
  for (int clientDescriptor : clientsDescriptor) {
    ssize_t send = write(clientDescriptor, dataPack.data->GetDataPtr(),
                         dataPack.data->GetSize());
    if (send < 0) {
      LOGD << "Closing connection with descriptor " << clientDescriptor;
      ipcSock.removeClientDescriptor(clientDescriptor);
    }
  }
}

}  // namespace - ml
