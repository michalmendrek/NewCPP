#ifndef DATFILEREADER_H
#define DATFILEREADER_H

#include "ISourceAdapter.h"
#include <TimerFramework/Worker.h>
#include <SensorData/Frame/V4LFrame.h>

#include <vector>
#include <string>
#include <fstream>
#include <map>

namespace ml {

class DatFileReader : public ISourceAdapter,
                      public Worker
{
public:
    DatFileReader(const std::vector<std::string> &datFiles,
                  const bool readInfinitely = false);

    // ISourceAdapter
    void Init() override;
    void Shutdown() override;
    void Start() override;
    void Stop() override;

    // Worker
    void Loop() override;

private:
    using IndiciesMap = std::map<long, std::streamsize>;
    using FramesVector = std::vector<std::shared_ptr<V4LFrame>>;
    bool OpenDatFile(const std::string &filePath);

    bool GetTimestampPeeks();
    FramesVector GetFramesAtTimestamp(const long timestamp);
    bool ReadIndicies(const std::string &indiciesFile);

    std::vector<std::string> mDatFiles;
    std::vector<std::ifstream> mDataStreams;
    bool mFilesOpened = false;
    std::vector<IndiciesMap> mIndicies;
    std::vector<IndiciesMap> mCopyIndicies;

    long mStartTimestamp = std::numeric_limits<long>::max();
    long mEndTimestamp = 0;

    bool mReadInfinitely = false;
};

} // namespace ml

#endif // DATFILEREADER_H
