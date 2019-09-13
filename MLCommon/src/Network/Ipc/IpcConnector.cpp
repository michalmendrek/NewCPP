#include "Network/Ipc/IpcConnector.h"
namespace ml {

IpcConnector::PrivateImpl::~PrivateImpl() {}

IpcConnector::IpcConnector(
    const std::vector<std::shared_ptr<IIpcSocket>>& sockets)
    : Worker("ipcWorker"),
      privateImpl(providePrivateImpl()),
      sockets(sockets) {}

void IpcConnector::Loop() { privateImpl->loop(sockets); }

void IpcConnector::Start() {
  if (isReady) {
    LOGW << "IpcConnector is already started";
    return;
  }

  for (const auto& socket : sockets) {
    if (not privateImpl->bindSocket(*socket)) {
      throw std::logic_error("Can't bind to " + socket->getSocketUri());
    }
  }

  privateImpl->start(sockets);

  Worker::Start();
  isReady = true;
}

void IpcConnector::Stop() {
  if (!isReady) {
    LOGW << "IpcConnector is already stopped";
    return;
  }

  StopWithJoin();
  for (const auto& socket : sockets) {
    privateImpl->closeSocket(*socket);
  }
  privateImpl->stop(sockets);

  isReady = false;
}

void IpcConnector::setUserGroup(const std::string& group) { userGroup = group; }
const std::string& IpcConnector::getUserGroup() const { return userGroup; }
}  // namespace ml
