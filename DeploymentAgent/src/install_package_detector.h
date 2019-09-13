#ifndef INSTALL_PACKAGE_DETECTOR_H_87631
#define INSTALL_PACKAGE_DETECTOR_H_87631

#include <plog/Log.h>
#include <functional>
#include <string>
#include <vector>
#include "archive_detector.h"

struct InstallPkg {
  InstallPkg() : rootPath(""), policyFile(""), archives({}) {}
  InstallPkg(const std::string& rootPath, const std::string& policyFile,
             const std::vector<std::string>& archives)
      : rootPath(addLastDelimiter(rootPath)),
        policyFile(policyFile),
        archives(archives) {}

  std::vector<std::string> allFiles(bool fullpath) const {
    std::vector<std::string> result{getArchivesPath(fullpath)};
    result.push_back(getPolicyPath(fullpath));
    return result;
  }

  void setRootPath(const std::string& path) {
    rootPath = addLastDelimiter(path);
  }

  const std::string& getRootPath() const { return rootPath; }
  std::string getPolicyPath(bool fullpath) const {
    return fullpath ? rootPath + policyFile : policyFile;
  }

  std::vector<std::string> getArchivesPath(bool fullpath) const {
    if (not fullpath) {
      return archives;
    }
    std::vector<std::string> result;
    result.reserve(archives.size());
    for (const std::string& arch : archives) {
      std::string finalPath{rootPath + arch};
      result.emplace_back(finalPath);
    }
    return result;
  }

 private:
  std::string rootPath;
  std::string policyFile;
  std::vector<std::string> archives;

  static std::string addLastDelimiter(const std::string& path) {
    return path.back() != '/' ? path + '/' : path;
  }
};

class InstallPackageDetector {
 public:
  bool process(const std::string& path) {
    auto policyFiles = findPolicyFile(path);
    if (policyFiles.empty()) {
      LOGE << "Expected policy file '" << POLICY_FILE
           << "' but not found, aborting";
      return false;
    }

    auto archFiles = findArchiveFile(path);
    if (archFiles.size() != 1) {
      LOGE << "Expected exactly one archive but " << archFiles.size()
           << " found, aborting";
      return false;
    }
    package = InstallPkg{path, policyFiles.front(), archFiles};
    return true;
  }

  const InstallPkg& getPackage() const { return package; }

 private:
  static std::string POLICY_FILE;
  InstallPkg package{};

  std::vector<std::string> findPolicyFile(const std::string& path) {
    FileNameDetector policyDetector{path, POLICY_FILE};
    return policyDetector.process();
  }

  std::vector<std::string> findArchiveFile(const std::string& path) {
    ArchiveDetector archDetector{path};
    return archDetector.process();
  }
};

std::string InstallPackageDetector::POLICY_FILE{"Policy.json"};

#endif  // ! INSTALL_PACKAGE_DETECTOR_H_87631
