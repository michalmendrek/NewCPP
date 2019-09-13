#ifndef ARCHIVE_DETECTOR_H_72AB563
#define ARCHIVE_DETECTOR_H_72AB563

#include <dirent.h>
#include <plog/Log.h>
#include <array>
#include <functional>
#include <string>
#include <vector>

class FileMatcher {
 public:
  using Matcher = std::function<bool(const std::string&)>;
  const std::string path;
  const Matcher matcher;

  FileMatcher(const std::string& path, const Matcher& matcher)
      : path(path), matcher(matcher) {}

  std::vector<std::string> process() const {
    LOGD << "Scanning path " << path;
    std::vector<std::string> result;
    DIR* dir{opendir(path.c_str())};
    if (dir != nullptr) {
      while (true) {
        struct dirent* ent = readdir(dir);
        if (ent == nullptr) {
          closedir(dir);
          break;
        }
        std::string filePath{ent->d_name};
        if (matcher(path + "/" + filePath)) {
          result.emplace_back(filePath);
          LOGD << "  Found '" << filePath << "'";
        }
      }
    }
    LOGD << "Scanning done.";
    return result;
  }
};

class FileNameDetector : public FileMatcher {
 public:
  FileNameDetector(const std::string& path, const std::string& filename)
      : FileMatcher(path, [=](const std::string& filePath) {
          return std::equal(filename.rbegin(), filename.rend(),
                            filePath.rbegin());
        }) {}
  operator bool() const { return process().size() > 0; }
};

class ExtensionMatcher {
 public:
  ExtensionMatcher(const std::string& extension) : extension(extension) {}
  bool operator()(const std::string& path) {
    return std::equal(extension.rbegin(), extension.rend(), path.rbegin());
  }

 private:
  std::string extension;
};

class StructureMatcher {
 public:
  bool operator()(const std::string& path) {
    if (not FileNameDetector{path, MODULE_CONFIG}) {
      return false;
    }
    if (not FileNameDetector{path, MODULE_IMPL}) {
      return false;
    }
    ExtensionMatcher extMatcher{MODULE_WEIGHTS_EXT};
    FileMatcher dirPrc{path, extMatcher};
    return dirPrc.process().size() == 1;
  }

 private:
  static const std::string MODULE_CONFIG;
  static const std::string MODULE_IMPL;
  static const std::string MODULE_WEIGHTS_EXT;
};
const std::string StructureMatcher::MODULE_CONFIG{"MLModule.json"};
const std::string StructureMatcher::MODULE_IMPL{"concrete_module.py"};
const std::string StructureMatcher::MODULE_WEIGHTS_EXT{".hdf5"};

class ArchiveDetector : public FileMatcher {
 public:
  enum class ArchiveType { UNKNOWN, ZIP, TAR, FOLDER };
  ArchiveDetector(const std::string& path) : FileMatcher(path, match) {}

  static ArchiveType identify(const std::string& path) {
    for (const IdentifyPair& p : archTypes) {
      if (p.second(path)) {
        return p.first;
      }
    }
    return ArchiveType::UNKNOWN;
  }

 private:
  using ArchiveMatcher = std::function<bool(const std::string& path)>;
  using IdentifyPair = std::pair<ArchiveType, ArchiveMatcher>;
  static const std::vector<IdentifyPair> archTypes;

  static bool match(const std::string& filePath) {
    for (const IdentifyPair& p : archTypes) {
      if (p.second(filePath)) {
        return true;
      }
    }
    return false;
  }
};

const std::vector<ArchiveDetector::IdentifyPair> ArchiveDetector::archTypes{
    {ArchiveDetector::ArchiveType::ZIP, ExtensionMatcher{".zip"}},
    {ArchiveDetector::ArchiveType::TAR, ExtensionMatcher{".tar"}},
    {ArchiveDetector::ArchiveType::FOLDER, StructureMatcher{}}};
#endif  // ! ARCHIVE_DETECTOR_H_72AB563
