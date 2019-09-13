#ifndef IFRAME_H
#define IFRAME_H

#include <Tools/Utils.h>
#include <Types/DataContainer.h>

#include "SensorData/Stream/IReadStream.h"
#include "SensorData/Stream/IWriteStream.h"

class IFrame {
 public:
  virtual ~IFrame() = default;
  virtual bool SerializeTo(IWriteStream &writer) = 0;
  virtual bool DeserializeFrom(IReadStream &reader) = 0;

  virtual uint64_t timestamp() const = 0;
  virtual const std::string &name() const = 0;
  virtual uint64_t frameSize() const = 0;
  virtual std::shared_ptr<ml::DataContainer> takeDataContainer() const = 0;
};

#endif  // IFRAME_H
