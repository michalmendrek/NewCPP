#include "DatFileWriter.h"

#include <plog/Log.h>

namespace ml {

static const unsigned short FramesCountToFlush = 10;

DatFileWriter::DatFileWriter(const std::string &rawDataFile, WritingStrategy strategy) :
  mRawDataFile{rawDataFile},
  mIndiciesFile{tools::removeFileExtension(rawDataFile) + ".idx"},
  mStrategy{strategy}
{
}

DatFileWriter::~DatFileWriter()
{
  if (mRawDataStream.is_open())
  {
    mRawDataStream.close();
  }
  if (mIndiciesStream.is_open())
  {
    mIndiciesStream.close();
  }
}

bool DatFileWriter::OpenFiles()
{
  if (mFilesOpened)
  {
    LOGD << "The data files are already opened";
    return true;
  }

  auto mode = std::ofstream::out | std::ofstream::binary;
  mode |= (mStrategy == WritingStrategy::Append ? std::ofstream::app : std::ofstream::trunc);

  try
  {
    mRawDataStream.open(mRawDataFile, mode);
    mIndiciesStream.open(mIndiciesFile, mode);
  }
  catch (const std::ofstream::failure& e)
  {
    mRawDataStream.close();
    mIndiciesStream.close();
    LOGE << "Couldn't open files. Explanatory string: " << e.what() << " Error code: " << e.code();
    return false;
  }

  mFilesOpened = true;
  return true;
}

void DatFileWriter::SaveMetaData(unsigned long timestamp, uint64_t frameSize)
{
  if (!mFilesOpened)
  {
    LOGE << "Dat files have not be opened";
    return;
  }

  mIndiciesStream.write(reinterpret_cast<char*>(&timestamp), sizeof(timestamp));
  mIndiciesStream.write(reinterpret_cast<char*>(&mRawDataStreamSize), sizeof(mRawDataStreamSize));
  mRawDataStreamSize += frameSize;
}

bool DatFileWriter::isFileOpened() const
{
  return mFilesOpened;
}

int DatFileWriter::Write(const void *data, size_t count)
{
  if (!mFilesOpened)
  {
    LOGE << "Dat files have not be opened";
    return -1;
  }

  mRawDataStream.write(static_cast<const char*>(data), count);

  mFramesWritten++;
  if (mFramesWritten == FramesCountToFlush)
  {
    mRawDataStream.flush();
    mIndiciesStream.flush();
    mFramesWritten = 0;
  }
  return static_cast<int>(count);
}

} // namespace ml
