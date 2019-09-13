#ifndef IIPC_LISTENER_H
#define IIPC_LISTENER_H

#include <Network/Ipc/IIpcSocket.h>
#include <Types/DataContainer.h>

namespace ml {

class IIpcListener {
 public:
  virtual ~IIpcListener() = default;
  virtual void dispatchData(const DataContainer& data,
                            const std::string& socketName,
                            const int descriptor) = 0;
};

}  // namespace ml

#endif  // ! IIPC_LISTENER_H
