#ifndef IREADSTREAM_H
#define IREADSTREAM_H

#include <Tools/Utils.h>

class IReadStream {
  ML_DECLARE_INTERFACE(IReadStream)
 public:
  virtual int Read(void *data, const size_t count) = 0;
};

#endif  // IREADSTREAM_H
