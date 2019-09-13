#include "DatFileReader.h"

#include <Tools/Utils.h>
#include <plog/Log.h>
#include <chrono>
#include <thread>

#include "FileReadStream.h"
#include "ISourceAdapterListener.h"

namespace ml {

static const int TimestampStep = 1000;

DatFileReader::DatFileReader(const std::vector<std::string> &datFiles,
                             const bool readInfinitely)
    : Worker("DatFileReader"),
      mDatFiles{datFiles},
      mReadInfinitely{readInfinitely} {}

void DatFileReader::Init() { LOGD << ""; }

void DatFileReader::Shutdown() { LOGD << ""; }

void DatFileReader::Start() {
  if (mDatFiles.empty()) {
    LOGE << "The dat files vector is empty";
    return;
  }

  for (const auto &datFile : mDatFiles) {
    if (!OpenDatFile(datFile)) {
      LOGE << "Couldn't open the dat file";
      return;
    }

    if (!ReadIndicies(tools::removeFileExtension(datFile) + ".idx")) {
      LOGE << "Couldn't open the corresponding indicies file";
      return;
    }
  }
  mFilesOpened = true;
  if (mReadInfinitely) {
    mCopyIndicies = mIndicies;
  }

  if (!GetTimestampPeeks()) {
    LOGE << "The indicies files are malformed";
    return;
  }

  Worker::Start();
}

void DatFileReader::Stop() {
  Worker::Stop();

  mFilesOpened = false;
  for (auto &dataStream : mDataStreams) {
    dataStream.close();
  }
  mDataStreams.clear();
}

void DatFileReader::Loop() {
  static const std::chrono::duration<int, std::milli> SleepBetweenSendingData(
      200);
  auto currentTimestamp = mStartTimestamp;
  while (currentTimestamp < mEndTimestamp) {
    const auto &frames = GetFramesAtTimestamp(currentTimestamp);
    currentTimestamp += TimestampStep;

    if (frames.empty()) {
      continue;
    }

    for (auto frame : frames) {
      mListener->ConsumeData(frame);
    }

    std::this_thread::sleep_for(SleepBetweenSendingData);
    // usleep(SleepBetweenSendingData);
  }

  if (mReadInfinitely) {
    mIndicies = mCopyIndicies;
  }
}

bool DatFileReader::OpenDatFile(const std::string &filePath) {
  std::ifstream rawDataStream;
  try {
    rawDataStream.open(filePath, std::ofstream::in | std::ofstream::binary);
  } catch (const std::ifstream::failure &e) {
    rawDataStream.close();
    LOGE << "Couldn't open files. Explanatory string: " << e.what()
         << " Error code: " << e.code();
    return false;
  }

  mDataStreams.emplace_back(std::move(rawDataStream));

  return true;
}

bool DatFileReader::GetTimestampPeeks() {
  if (mIndicies.empty()) {
    return false;
  }

  for (const auto &indiciesMap : mIndicies) {
    if (indiciesMap.empty()) {
      continue;
    }

    mStartTimestamp = std::min(indiciesMap.begin()->first, mStartTimestamp);
    mEndTimestamp = std::max(indiciesMap.rbegin()->first, mEndTimestamp);
  }
  return true;
}

DatFileReader::FramesVector DatFileReader::GetFramesAtTimestamp(
    const long timestamp) {
  FramesVector frames;
  for (size_t index = 0; index < mIndicies.size(); ++index) {
    auto &mIndiciesMap = mIndicies.at(index);
    auto bound = mIndiciesMap.lower_bound(timestamp);
    if (bound == mIndiciesMap.begin()) {
      continue;
    }
    bound--;
    std::for_each(mIndiciesMap.begin(), bound,
                  [this, &frames, &mIndiciesMap,
                   index](const std::pair<long, std::streamsize> &pair) {
                    auto frame = std::make_shared<V4LFrame>();
                    FileReadStream readStream(mDataStreams[index], pair.second);
                    if (!frame->DeserializeFrom(readStream)) {
                      return;
                    }
                    mIndiciesMap.erase(pair.first);
                    frames.emplace_back(frame);
                  });
  }

  return frames;
}

bool DatFileReader::ReadIndicies(const std::string &indiciesFile) {
  std::ifstream indiciesDataStream;
  try {
    indiciesDataStream.open(indiciesFile,
                            std::ofstream::in | std::ofstream::binary);
  } catch (const std::ifstream::failure &e) {
    indiciesDataStream.close();
    LOGE << "Couldn't open files. Explanatory string: " << e.what()
         << " Error code: " << e.code();
    return false;
  }

  long timestamp;
  std::streamsize frameSize;
  IndiciesMap map;
  while (indiciesDataStream.good()) {
    indiciesDataStream.read(reinterpret_cast<char *>(&timestamp),
                            sizeof(timestamp));
    indiciesDataStream.read(reinterpret_cast<char *>(&frameSize),
                            sizeof(frameSize));
    map.insert({timestamp, frameSize});
  }

  indiciesDataStream.close();
  mIndicies.emplace_back(std::move(map));
  return true;
}

}  // namespace ml
