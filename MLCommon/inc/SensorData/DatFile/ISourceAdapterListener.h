#ifndef ISOURCEADAPTERLISTENER_H
#define ISOURCEADAPTERLISTENER_H

#include <SensorData/Frame/V4LFrame.h>
#include <Tools/Utils.h>
#include <stdint.h>

namespace ml {

class ISourceAdapterListener {
  ML_DECLARE_INTERFACE(ISourceAdapterListener)
 public:
  virtual void ConsumeData(std::shared_ptr<IFrame> frame) = 0;
};

}  // namespace ml

#endif  // ! ISOURCEADAPTERLISTENER_H
