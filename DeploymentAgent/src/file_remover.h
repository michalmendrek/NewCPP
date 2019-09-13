#ifndef FILE_REMOVER_H_677667A
#define FILE_REMOVER_H_677667A

#include <plog/Log.h>
#include <stdio.h>
#include <string>
#include <vector>

class FileRemover {
 public:
  bool removeFiles(const std::vector<std::string>& files) {
    for (const std::string& src : files) {
      LOGI << "Removing '" << src << '\'';
      if (remove(src.c_str()) != 0) {
        LOGE << "Can't remove '" << src << "' error: " << errno;
        LOGE << "Operation aborted.";
        return false;
      }
    }
    return true;
  }
};

#endif  // ! FILE_REMOVER_H_677667A
