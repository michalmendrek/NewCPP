#ifndef IWRITESTREAM_H
#define IWRITESTREAM_H

#include <Tools/Utils.h>

class IWriteStream {
  ML_DECLARE_INTERFACE(IWriteStream)
 public:
  virtual int Write(const void *data, size_t count) = 0;
};

#endif  // IWRITESTREAM_H
