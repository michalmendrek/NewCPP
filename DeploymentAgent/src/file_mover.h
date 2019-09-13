#ifndef FILE_MOVER_H_213b32
#define FILE_MOVER_H_213b32

#include <plog/Log.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "system_cmd_executor.h"

class FileMover {
 public:
  static bool moveFile(const std::string& srcPath, const std::string& destPath,
                       const bool overwriteDest) {
    if (overwriteDest) {
      remove(destPath.c_str());
    }
    return moveFile(srcPath, destPath);
  }

  static bool moveFiles(const std::vector<std::string>& files,
                        const std::string& destPath) {
    if (destPath.empty() or files.empty()) {
      LOGE << "Move imposible, destPath is '' or no files to copy";
      return false;
    }

    for (const std::string& src : files) {
      const std::string& finalDestPath = combine(src, destPath);
      if (finalDestPath.empty()) {
        LOGE << "Unable to combine '" << destPath << "', and '" << src
             << "', abort move.";
        return false;
      }

      if (not moveFile(src, finalDestPath)) {
        LOGE << "Can't move '" << src << "' into '" << finalDestPath
             << "', abort move.";
        return false;
      }
    }

    return true;
  }

 private:
  static std::string combine(const std::string& src,
                             const std::string& destPath) {
    size_t index = src.find_last_of('/');
    if (index == std::string::npos) {
      return joinComponents(destPath, src);

    } else {
      if (index < src.size() - 1) {
        return joinComponents(destPath, src.substr(index + 1));
      } else {
        return {};
      }
    }
  }

  static std::string joinComponents(const std::string& prefixPath,
                                    const std::string& filename) {
    if (prefixPath.back() == '/') {
      return prefixPath + filename;
    } else {
      return prefixPath + '/' + filename;
    }
  }

  static bool moveFile(const std::string& srcPath,
                       const std::string& destPath) {
    LOGI << "Moving '" << srcPath << "' into '" << destPath << '\'';
    int status = rename(srcPath.c_str(), destPath.c_str());
    if (status == -1) {
      int err = errno;
      if (err == EXDEV) {
        LOGI << "Can't move in one file system, fallback to copy.";
        return copyFile(srcPath, destPath);
      } else {
        LOGI << "Move error:" << err;
      }
    }
    return status == 0;
  }

  static bool copyFile(const std::string& srcPath,
                       const std::string& destPath) {
    bool copied =
        SystemCmdExecutor{"cp -rf '" + srcPath + "' '" + destPath + "'"};
    if (copied) {
      remove(srcPath.c_str());
    } else {
      LOGW << "Copy returned error, source is not removed";
    }
    return copied;
  }
};

#endif  // ! FILE_MOVER_H_213b32
