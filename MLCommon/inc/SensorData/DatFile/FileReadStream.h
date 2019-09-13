#ifndef FILEREADSTREAM_H
#define FILEREADSTREAM_H

#include <SensorData/Stream/IReadStream.h>
#include <fstream>

class FileReadStream : public IReadStream
{
public:
    FileReadStream(std::ifstream &fileStream, std::streamsize position);

    // IReadStream
    int Read(void *data, const size_t count) override;

private:
    std::ifstream &mFileStream;
    std::streamsize mPosition = 0;
};

#endif // FILEREADSTREAM_H
