#ifndef V4LFRAME_H
#define V4LFRAME_H

#include <Types/DataContainer.h>
#include "Configuration/iconfigmanager.h"
#include "Converter/converterconstants.h"
#include "IFrame.h"

#include <vector>

namespace ml {

class V4LFrame : public IFrame {
 public:
  bool SerializeTo(IWriteStream &writer) override;
  bool DeserializeFrom(IReadStream &reader) override;

  bool AsyncDeserializeFrom(IReadStream &reader);

  void setTimestamp(const uint64_t timestamp);
  void setData(char *data, uint64_t size);
  void setName(const std::string &name);
  void setInputWidth(const uint32_t width);
  void setInputHeight(const uint32_t height);
  void setBytesPerLine(const uint32_t bytesPerLine);
  void setBitsPerPixel(const uint8_t bitsPerPixel);
  void setColorFormat(const uint8_t colorFormat);

  uint64_t timestamp() const override;
  const std::string &name() const override;
  uint64_t frameSize() const override;
  std::shared_ptr<DataContainer> takeDataContainer() const override;

 private:
  void prepareHeader();
  void updateFrame();
  bool parseIFrameHeader(IReadStream &reader);
  bool parseV4LFrameHeader(IReadStream &reader);

  enum class State { IF_HEADER, V4L_HEADER, V4L_BODY, DESERIALIZED };

  std::shared_ptr<DataContainer> mDataContainer =
      std::make_shared<DataContainer>();

  uint64_t mTimestamp = 0;
  std::string mName;
  uint32_t mInputWidth = 0;
  uint32_t mInputHeight = 0;
  uint32_t mBytesPerLine = 0;
  uint8_t mBitsPerPixel = 0;
  uint8_t mColorFormat = 0;
  std::vector<char> mHeader;
  size_t mVideoOffset = 0;
  State state{State::IF_HEADER};
  size_t accumulated{0};
};

}  // namespace ml

#endif  // V4LFRAME_H
