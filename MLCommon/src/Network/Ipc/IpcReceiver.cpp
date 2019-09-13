#include "Network/Ipc/IpcReceiver.h"

namespace ml {

IpcReceiver::PrivateImpl::~PrivateImpl() {}

IpcReceiver::IpcReceiver(const SocketVector& sockets, IIpcListener& listener,
                         const size_t sizeOfBuffer)
    : Worker("IpcReceiver"),
      sockets(sockets),
      listener(listener),
      sizeOfBuffer(sizeOfBuffer),
      privateImpl(providePrivateImpl()) {
  buffer.resize(sizeOfBuffer);
}

void IpcReceiver::Loop() { privateImpl->loop(sockets, buffer, listener); }

}  // namespace ml
