#ifndef IPCSOCKETUNIX_C78AFD
#define IPCSOCKETUNIX_C78AFD
#include "Network/Ipc/IIpcSocket.h"

namespace ml {
class IpcSocketUnix : public IIpcSocket, public IpcGenericSocket<int> {
 public:
  std::string socketPath;

  IpcSocketUnix(const std::string& uri, const std::string& name);
  ~IpcSocketUnix();
};

}  // namespace ml
#endif  //! IPCSOCKETUNIX_C78AFD
