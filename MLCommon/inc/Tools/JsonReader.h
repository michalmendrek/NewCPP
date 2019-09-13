#ifndef JSONREADER_H
#define JSONREADER_H

#include <string>

#include <picojson.h>

namespace ml {

class JsonReader {
 public:
  JsonReader() = default;

  bool ReadContent(const std::string &filePath);

  template <typename T>
  const T &root() const {
    return mContent.get<T>();
  }

  bool contains(const picojson::object &obj, const std::string &key) const;

 private:
  picojson::value mContent;
};

}  // namespace ml

#endif  // JSONREADER_H
