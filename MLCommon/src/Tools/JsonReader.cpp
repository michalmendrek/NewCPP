#include "JsonReader.h"

#include <fstream>

#include <plog/Log.h>

namespace ml {

bool JsonReader::ReadContent(const std::string &filePath) {
  try {
    std::ifstream fileStream;
    fileStream.open(filePath, std::ios::binary);

    if (fileStream.is_open() && fileStream.good()) {
      fileStream >> mContent;
      fileStream.close();
      return picojson::get_last_error().empty();
    } else {
      LOGE << "Couldn't read from the file: " << filePath;
      return false;
    }
  } catch (const std::ofstream::failure &e) {
    LOGE << "Couldn't open file. Explanatory string: " << e.what()
         << " Error code: " << e.code();
    return false;
  }
}

bool JsonReader::contains(const picojson::object &obj,
                          const std::string &key) const {
  return obj.find(key) != obj.end();
}

}  // namespace ml
