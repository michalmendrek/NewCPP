#ifndef ISSTREAMADAPTERLISTENER_H
#define ISTREAMADAPTERLISTENER_H

#include <Tools/Utils.h>

#include <string>

namespace ml {

class IStreamAdapterListener {
  ML_DECLARE_INTERFACE(IStreamAdapterListener)
 public:
  virtual void ConsumeData(const std::string& streamName, void* data,
                           const size_t len) = 0;
};

}  // namespace ml

#endif  // ! ISOURCEADAPTERLISTENER_H
