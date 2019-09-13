#ifndef DATFILEWRITER_H
#define DATFILEWRITER_H

#include <string>
#include <fstream>

#include <Tools/Utils.h>
#include <SensorData/Stream/IWriteStream.h>

namespace ml {

class DatFileWriter : public IWriteStream
{
  DISABLE_COPY_MOVE(DatFileWriter)
public:
  enum class WritingStrategy {
    Append,
    Truncate
  };
  DatFileWriter(const std::string &rawDataFile, WritingStrategy strategy = WritingStrategy::Append);
  ~DatFileWriter();

  bool OpenFiles();
  void SaveMetaData(unsigned long timestamp, uint64_t frameSize);
  bool isFileOpened(void) const;

  // IWriteStream
  int Write(const void *data, size_t count);

private:
  const std::string mRawDataFile;
  const std::string mIndiciesFile;

  std::ofstream mRawDataStream;
  std::ofstream mIndiciesStream;

  std::streamsize mRawDataStreamSize = 0;
  unsigned short mFramesWritten = 0;
  bool mFilesOpened = false;

  WritingStrategy mStrategy = WritingStrategy::Append;
};

}

#endif // DATFILEWRITER_H
