#include "IpcSocketWindows.h"

#include <exception>

#include "Network/Ipc/NaiveURIParser.h"

namespace ml {

IpcSocketWindows::IpcSocketWindows(const std::string& uri,
                                   const std::string &name) : IIpcSocket(uri, name) {
  NaiveURIParser parsedUri(uri);
  if (not parsedUri) {
    throw std::runtime_error("Malformed Endpoint Uri=" + uri);
  }
  if (parsedUri.protocol != "tcp") {
    throw std::runtime_error("Only TCP is supported as IPC");
  }

  if (parsedUri.host.empty()) {
    throw std::runtime_error("'Host' part of URI is expected");
  }
  if (parsedUri.port.empty()) {
    throw std::runtime_error("'Port' part of URI is expected");
  }

  port = static_cast<unsigned short>(std::stoi(parsedUri.port));
  host = parsedUri.host;
}

IpcSocketWindows::~IpcSocketWindows() {}

std::shared_ptr<IIpcSocket> IIpcSocket::create(const std::string& uri,
                                               const std::string &name) {
  return std::make_shared<IpcSocketWindows>(uri, name);
}

}  // namespace ml
