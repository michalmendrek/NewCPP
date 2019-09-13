#include "V4LFrame.h"

#include "Converter/converterutils.h"

#include <plog/Log.h>

namespace {

bool fillupBuffer(IReadStream &reader, void *buf, size_t bufSize,
                  size_t &actSize) {
  while (actSize < bufSize) {
    size_t amount = bufSize - actSize;
    char *ptr = reinterpret_cast<char *>(buf) + actSize;
    int result = reader.Read(ptr, amount);
    if (result <= 0) {
      return false;
    }
    actSize += static_cast<size_t>(result);
  }
  return true;
}

}  // anonymous namespace

namespace ml {

bool V4LFrame::SerializeTo(IWriteStream &writer) {
  updateFrame();
  auto bytesWritten = writer.Write(mHeader.data(), mHeader.size());
  bytesWritten +=
      writer.Write(mDataContainer->GetDataPtr(), mDataContainer->GetSize());
  return bytesWritten ==
         static_cast<int>(mDataContainer->GetSize() + mHeader.size());
}

bool V4LFrame::parseIFrameHeader(IReadStream &reader) {
  static constexpr size_t BUFFER_SIZE{8};
  if (mHeader.size() != BUFFER_SIZE) {
    mHeader.resize(BUFFER_SIZE);
  }
  if (fillupBuffer(reader, mHeader.data(), mHeader.size(), accumulated)) {
    const auto frameSize =
        ConverterUtils::getU32(mHeader, converter::OFFSET_FRAME_SIZE);
    const auto headerSize =
        ConverterUtils::getU32(mHeader, converter::OFFSET_HEADER_SIZE) + 4 +
        converter::VIDEO_HEADER_LENGTH;
    const auto dataSize = frameSize - headerSize;
    const auto data = new char[dataSize];
    mDataContainer->SetData(data, dataSize);
    mVideoOffset = headerSize - converter::VIDEO_HEADER_LENGTH;
    mHeader.resize(headerSize);
    state = State::V4L_HEADER;
    return true;
  }
  return false;
}

bool V4LFrame::parseV4LFrameHeader(IReadStream &reader) {
  if (fillupBuffer(reader, mHeader.data(), mHeader.size(), accumulated)) {
    mTimestamp = ConverterUtils::getU64(mHeader, converter::OFFSET_TIMESTAMP);
    const auto &nameLength = mHeader[converter::OFFSET_NAME_LENGTH];
    mName.assign(&mHeader[converter::FRAME_HEADER_LENGTH],
                 static_cast<std::string::size_type>(nameLength));

    mInputWidth = ConverterUtils::getU32(
        mHeader, mVideoOffset + converter::OFFSET_VIDEO_WIDTH);
    mInputHeight = ConverterUtils::getU32(
        mHeader, mVideoOffset + converter::OFFSET_VIDEO_HEIGHT);
    mBytesPerLine = ConverterUtils::getU32(
        mHeader, mVideoOffset + converter::OFFSET_VIDEO_BYTES_PER_LINE);
    mBitsPerPixel = static_cast<uint8_t>(
        mHeader[mVideoOffset + converter::OFFSET_VIDEO_BITS_PER_PIXEL]);
    mColorFormat = static_cast<uint8_t>(
        mHeader[mVideoOffset + converter::OFFSET_VIDEO_COLOR_FORMAT]);
    state = State::V4L_BODY;
    accumulated = 0;
    return true;
  }
  return false;
}

bool V4LFrame::AsyncDeserializeFrom(IReadStream &reader) {
  while (true) {
    switch (state) {
      case State::IF_HEADER:
        if (parseIFrameHeader(reader)) {
          continue;
        }
        return false;

      case State::V4L_HEADER:
        if (parseV4LFrameHeader(reader)) {
          continue;
        }
        return false;

      case State::V4L_BODY: {
        bool result = fillupBuffer(
            reader, const_cast<char *>(mDataContainer->GetDataPtr()),
            mDataContainer->GetSize(), accumulated);
        if (result) {
          state = State::DESERIALIZED;
        }
        return result;
      }
      case State::DESERIALIZED:
        throw std::runtime_error("This frame is already deserialized");
    }
  }
}

bool V4LFrame::DeserializeFrom(IReadStream &reader) {
  uint32_t bytesRead = 0;
  static const unsigned int FrameAndHeaderSizeBytes = 8;
  mHeader.resize(FrameAndHeaderSizeBytes);
  do {
    const auto read = reader.Read(mHeader.data() + bytesRead,
                                  FrameAndHeaderSizeBytes - bytesRead);
    if (read < 0) {
      LOGE << "Couldn't read frame data";
      return false;
    }
    bytesRead += read;
  } while (bytesRead != FrameAndHeaderSizeBytes);

  const auto frameSize =
      ConverterUtils::getU32(mHeader, converter::OFFSET_FRAME_SIZE);
  const auto headerSize =
      ConverterUtils::getU32(mHeader, converter::OFFSET_HEADER_SIZE) + 4 +
      converter::VIDEO_HEADER_LENGTH;
  const auto dataSize = frameSize - headerSize;
  const auto data = new char[dataSize];
  mDataContainer->SetData(data, dataSize);
  mVideoOffset = headerSize - converter::VIDEO_HEADER_LENGTH;
  mHeader.resize(headerSize);

  do {
    const auto read =
        reader.Read(mHeader.data() + bytesRead, headerSize - bytesRead);
    if (read < 0) {
      LOGE << "Couldn't read frame data";
      return false;
    }
    bytesRead += read;
  } while (bytesRead != headerSize);

  mTimestamp = ConverterUtils::getU64(mHeader, converter::OFFSET_TIMESTAMP);
  const auto &nameLength = mHeader[converter::OFFSET_NAME_LENGTH];
  mName.assign(&mHeader[converter::FRAME_HEADER_LENGTH], nameLength);

  mInputWidth = ConverterUtils::getU32(
      mHeader, mVideoOffset + converter::OFFSET_VIDEO_WIDTH);
  mInputHeight = ConverterUtils::getU32(
      mHeader, mVideoOffset + converter::OFFSET_VIDEO_HEIGHT);
  mBytesPerLine = ConverterUtils::getU32(
      mHeader, mVideoOffset + converter::OFFSET_VIDEO_BYTES_PER_LINE);
  mBitsPerPixel = static_cast<uint8_t>(
      mHeader[mVideoOffset + converter::OFFSET_VIDEO_BITS_PER_PIXEL]);
  mColorFormat = static_cast<uint8_t>(
      mHeader[mVideoOffset + converter::OFFSET_VIDEO_COLOR_FORMAT]);

  bytesRead = 0;
  do {
    const auto read = reader.Read(data + bytesRead, dataSize - bytesRead);
    if (read < 0) {
      LOGE << "Couldn't read frame data";
      return false;
    }
    bytesRead += read;
  } while (bytesRead != dataSize);

  return true;
}

void V4LFrame::setTimestamp(uint64_t timestamp) { mTimestamp = timestamp; }

void V4LFrame::setData(char *data, uint64_t size) {
  mDataContainer->SetData(data, size);
}

void V4LFrame::setName(const std::string &name) {
  mName = name;
  prepareHeader();
}

void V4LFrame::setInputWidth(const uint32_t width) { mInputWidth = width; }

void V4LFrame::setInputHeight(const uint32_t height) { mInputHeight = height; }

void V4LFrame::setBytesPerLine(const uint32_t bytesPerLine) {
  mBytesPerLine = bytesPerLine;
}

void V4LFrame::setBitsPerPixel(const uint8_t bitsPerPixel) {
  mBitsPerPixel = bitsPerPixel;
}

void V4LFrame::setColorFormat(const uint8_t colorFormat) {
  mColorFormat = colorFormat;
}

uint64_t V4LFrame::timestamp() const { return mTimestamp; }

const std::string &V4LFrame::name() const { return mName; }

uint64_t V4LFrame::frameSize() const {
  return mDataContainer->GetSize() + mHeader.size();
}

std::shared_ptr<DataContainer> V4LFrame::takeDataContainer() const {
  return mDataContainer;
}

void V4LFrame::prepareHeader() {
  auto nameLength = static_cast<uint8_t>(
      std::min(mName.length(), converter::MAX_NAME_LENGTH));
  mVideoOffset = converter::FRAME_HEADER_LENGTH +
                 ConverterUtils::getPaddedSize(nameLength);
  mHeader.resize(mVideoOffset + converter::VIDEO_HEADER_LENGTH);
  std::fill(mHeader.begin(), mHeader.end(), 0);

  ConverterUtils::setU32(mHeader, converter::OFFSET_HEADER_SIZE,
                         static_cast<uint32_t>(mVideoOffset) - 4);
  mHeader[converter::OFFSET_HEADER_VERSION] = converter::HEADER_VERSION;
  mHeader[converter::OFFSET_HEADER_SUBVERSION] = converter::HEADER_SUBVERSION;
  mHeader[converter::OFFSET_NAME_LENGTH] = static_cast<char>(nameLength);
  mHeader[converter::OFFSET_RESERVED_1] = 0;
  ConverterUtils::setU32(mHeader, converter::OFFSET_MEDIA_TYPE,
                         converter::MEDIA_TYPE_VIDEO);
  ConverterUtils::setU32(mHeader, converter::OFFSET_MEDIA_SUBTYPE,
                         converter::MEDIA_SUBTYPE_VIDEO_UNCOMPRESSED);

  mName.copy(reinterpret_cast<char *>(&mHeader[converter::FRAME_HEADER_LENGTH]),
             nameLength, 0);
}

void V4LFrame::updateFrame() {
  const auto frameSize = mHeader.size() + mDataContainer->GetSize();
  // TODO: figure out which ones are fixed and can be set once and which ones
  // can be changed during execution and have to update each time before
  // sending.
  ConverterUtils::setU32(mHeader, converter::OFFSET_FRAME_SIZE, frameSize);
  ConverterUtils::setU64(mHeader, converter::OFFSET_TIMESTAMP,
                         static_cast<uint64_t>(mTimestamp));
  ConverterUtils::setU32(mHeader, mVideoOffset + converter::OFFSET_VIDEO_WIDTH,
                         mInputWidth);
  ConverterUtils::setU32(mHeader, mVideoOffset + converter::OFFSET_VIDEO_HEIGHT,
                         mInputHeight);
  ConverterUtils::setU32(mHeader,
                         mVideoOffset + converter::OFFSET_VIDEO_BYTES_PER_LINE,
                         mBytesPerLine);
  mHeader[mVideoOffset + converter::OFFSET_VIDEO_BITS_PER_PIXEL] =
      static_cast<char>(mBitsPerPixel);
  mHeader[mVideoOffset + converter::OFFSET_VIDEO_COLOR_FORMAT] =
      static_cast<char>(mColorFormat);
}

}  // namespace ml
