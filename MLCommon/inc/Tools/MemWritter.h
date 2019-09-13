#ifndef MEMWRITTER_H_92834A
#define MEMWRITTER_H_92834A

#include <SensorData/Stream/IWriteStream.h>

namespace ml {

class MemWritter : public IWriteStream {
 public:
  MemWritter(uint8_t* bufferPtr, size_t size)
      : bufferPtr(bufferPtr), size(size) {}

  virtual int Write(const void* data, size_t count) override;

 private:
  uint8_t* bufferPtr;
  size_t size;
  size_t position{0};
};
}
#endif  //! MEMWRITTER_H_92834A
