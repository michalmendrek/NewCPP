#include "FileReadStream.h"

FileReadStream::FileReadStream(std::ifstream &fileStream, std::streamsize position) :
    mFileStream{fileStream},
    mPosition{position}
{
    mFileStream.seekg(position);
}

int FileReadStream::Read(void *data, const size_t count)
{
    mFileStream.read(static_cast<char*>(data), static_cast<std::streamsize>(count));
    return static_cast<int>(mFileStream.gcount());
}
