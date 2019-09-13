#ifndef IIPCSOCKETWINDOWS_H_28A934
#define IIPCSOCKETWINDOWS_H_28A934

#include <winsock2.h>
#include <ws2tcpip.h>

#include <string>

#include "Network/Ipc/IIpcSocket.h"

namespace ml {
class IpcSocketWindows : public IIpcSocket, public IpcGenericSocket<SOCKET> {
 public:
  unsigned short port{};
  std::string host;

  IpcSocketWindows(const std::string& uri, const std::string &name);
  ~IpcSocketWindows();
};

}  // namespace ml
#endif  // IIPCSOCKETWINDOWS_H_28A934
