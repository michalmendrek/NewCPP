#ifndef UNPACKER_H_2A1E34
#define UNPACKER_H_2A1E34

#include <plog/Log.h>
#include <stdlib.h>
#include <functional>
#include <string>
#include <vector>
#include "archive_detector.h"
#include "system_cmd_executor.h"

class Unpacker {
 public:
  bool unpack(const std::string& archivePath, const std::string& destPath) {
    ArchiveDetector::ArchiveType archType{
        ArchiveDetector::identify(archivePath)};
    if (archType == ArchiveDetector::ArchiveType::UNKNOWN) {
      return false;
    }
    if (characterExist(archivePath, '\'') or characterExist(destPath, '\'')) {
      LOGE << "There is forbiden character ' in path";
      return false;
    }
    auto it =
        std::find_if(builders.begin(), builders.end(),
                     [=](const Builder& b) { return b.first == archType; });
    std::string finalDestPath{destPath + (destPath.back() == '/' ? "" : "/") +
                              lastNameComponent(archivePath)};
    const std::string& cmd = it->second(archivePath, finalDestPath);
    return SystemCmdExecutor{cmd};
  }

 private:
  using CmdBuilder = std::function<std::string(const std::string& archivePath,
                                               const std::string& destPath)>;
  using Builder = std::pair<ArchiveDetector::ArchiveType, CmdBuilder>;
  static const std::vector<Builder> builders;

  static bool characterExist(const std::string& str, char c) {
    return str.find(c) != std::string::npos;
  }

  static std::string lastNameComponent(const std::string& path) {
    std::string result{path};
    if (result.back() == '/') {
      result.erase(std::prev(result.end()));
    }
    size_t index = result.find_last_of('/');
    if (index == std::string::npos) {
      return result;
    }
    result = result.substr(index + 1);
    index = result.find_last_of('.');
    if (index == std::string::npos) {
      return result;
    }
    return result.substr(0, index);
  }

  static std::string zipBuilder(const std::string& archivePath,
                                const std::string& destPath) {
    return "unzip -qo '" + archivePath + "' -d '" + destPath + "'";
  }

  static std::string tarBuilder(const std::string& archivePath,
                                const std::string& destPath) {
    return "mkdir -p '" + destPath + "' && tar -C '" + destPath + "' -xf '" +
           archivePath + "'";
  }
  static std::string folderBuilder(const std::string& archivePath,
                                   const std::string& destPath) {
    return "cp -fr '" + archivePath + "' '" + destPath + "'";
  }
};

const std::vector<Unpacker::Builder> Unpacker::builders{
    {ArchiveDetector::ArchiveType::ZIP, Unpacker::zipBuilder},
    {ArchiveDetector::ArchiveType::TAR, Unpacker::tarBuilder},
    {ArchiveDetector::ArchiveType::FOLDER, Unpacker::folderBuilder}};
#endif  // ! UNPACKER_H_2A1E34
