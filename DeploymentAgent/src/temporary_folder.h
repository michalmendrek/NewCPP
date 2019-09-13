#ifndef TEMPORARY_FOLDER_H_86BC34A
#define TEMPORARY_FOLDER_H_86BC34A

#include <ftw.h>
#include <plog/Log.h>
#include <stdio.h>
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <random>
#include <string>

class TemporaryFolder {
 public:
  TemporaryFolder(const std::string &parent) {
    if (parent.empty()) {
      generateFolder("/tmp");
    } else {
      std::ifstream fileStream(parent);
      if (not fileStream.good()) {
        throw std::runtime_error("Can't locate '" + parent + "'");
      }
      generateFolder(parent);
    }
  }

  ~TemporaryFolder() {
    if (not fullPath.empty()) {
      travelsalRemove();
    }
  }

  operator bool() const { return not fullPath.empty(); }
  const std::string &getFullPath() const { return fullPath; }

 private:
  std::string fullPath;

  void generateFolder(const std::string parent) {
    static constexpr size_t NAME_LEN = 10;
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> distrib('A', 'Z');
    std::string randomName;
    for (size_t t = 0; t < NAME_LEN; t++) {
      randomName += static_cast<char>(distrib(rng));
    }
    fullPath = parent + (parent.back() == '/' ? "" : "/") + randomName;
    int status = mkdir(fullPath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (status < 0) {
      fullPath = "";
      LOGI << "Unable to create temp folder at " << fullPath;
    } else {
      LOGI << "New temporary folder created at " << fullPath;
    }
  }

  void travelsalRemove() {
    constexpr int MAX_HOLD_OPEN_DESCRIPTORS = 10;

    if (nftw(fullPath.c_str(), removeSingleFile, MAX_HOLD_OPEN_DESCRIPTORS,
             FTW_DEPTH | FTW_MOUNT | FTW_PHYS) < 0) {
      LOGE << "Error while removing temporary folder:" << fullPath;
    }
  }

  static int removeSingleFile(const char *pathname, const struct stat *sbuf,
                              int type, struct FTW *ftwb) {
    (void)sbuf;
    (void)type;
    (void)ftwb;
    return remove(pathname);
  }
};

#endif  // ! TEMPORARY_FOLDER_H_86BC34A
