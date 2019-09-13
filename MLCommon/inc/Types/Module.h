#ifndef TYPES_MODULE_H
#define TYPES_MODULE_H

#include <list>
#include <string>
#include <vector>

namespace Types {

class Module {
 public:
  struct SensorStream {
    std::string name;
    size_t cacheSize;
  };
  using LibraryList = std::list<std::string>;

  Module(const std::string& name);
  virtual ~Module() = default;

  void AddLibrary(const std::string& val);

  void AddMandatoryData(const SensorStream& val);
  const std::vector<SensorStream>& MandatoryData() const;
  bool IsStreamMandatory(const std::string& name) const;

  const LibraryList& GetConstLibraryList() const;

  const std::string& GetConstName() const;

  void SetModulePath(const std::string& val);
  const std::string& GetConstModulePath() const;

  void SetStarted();
  void ClearStarted();
  const bool& GetStarted() const;

  void SetConnectedOK();
  void ClearConnectedOK();
  const bool& GetConnectedOK() const;

  const std::string& GetOutputData() const;
  void SetOutputData(const std::string& name);

 private:
  std::string mName;
  std::string mModulePath;
  std::string mOutputData{""};
  std::vector<SensorStream> mMandatoryData;
  LibraryList mLibraryList;
  bool mStarted = false;
  bool mConnectedOK = false;
};

inline bool operator==(const Module& lhs, const Module& rhs) {
  return lhs.GetConstName() == rhs.GetConstName();
}

}  // namespace - Types

#endif  // ! TYPES_MODULE_H
