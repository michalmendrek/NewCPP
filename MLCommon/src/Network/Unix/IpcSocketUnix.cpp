#include "IpcSocketUnix.h"
#include <exception>
#include "Network/Ipc/NaiveURIParser.h"

namespace ml {
IpcSocketUnix::~IpcSocketUnix() {}

IpcSocketUnix::IpcSocketUnix(const std::string& uri, const std::string& name)
    : IIpcSocket(uri, name) {
  NaiveURIParser parsedUri(uri);
  if (not parsedUri) {
    throw std::runtime_error("Malformed Endpoint Uri=" + uri);
  }
  if (parsedUri.protocol != "unix") {
    throw std::runtime_error("Only UnixSocket is supported as IPC");
  }
  if (parsedUri.path.empty()) {
    throw std::runtime_error("Path component of URI can't be empty");
  }
  socketPath = parsedUri.path;
}

std::shared_ptr<IIpcSocket> IIpcSocket::create(const std::string& uri,
                                               const std::string& name) {
  return std::make_shared<IpcSocketUnix>(uri, name);
}

}  // namespace ml
