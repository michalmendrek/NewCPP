#include "Utils.h"

#include <libgen.h>
#include <unistd.h>

#include <chrono>

#ifdef __linux__
#include <dirent.h>
#include <linux/limits.h>
#elif __WIN32__ || __WIN64__
#include <windows.h>

#include <unistd.h>
#include "Shlwapi.h"

#define stat _stat

#endif

#include <plog/Log.h>

namespace tools {

bool endsWith(const std::string &str, const std::string &suffix) {
  return (str.size() >= suffix.size()) &&
         (str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0);
}

/*!
 * \brief timestamp - returns current time in milliseconds.
 * \return
 */
long timestamp() {
  auto ret = std::chrono::duration_cast<std::chrono::milliseconds>(
                 std::chrono::system_clock::now().time_since_epoch())
                 .count();
  return static_cast<long>(ret);
}

/*!
 * \brief removeFileExtension - returns original string without extension.
 * \param filePath
 * \return
 */
std::string removeFileExtension(const std::string &filePath) {
  const auto pos = filePath.find_last_of(".");
  if (pos == std::string::npos) {
    return filePath;
  }
  return filePath.substr(0, pos);
}

/*!
 * \brief getExecutableDir - returns the directory path where executable
 * application is located. \return
 */
std::string getExecutableDir() {
#ifdef __linux__
  char executablePath[PATH_MAX];
  if (readlink("/proc/self/exe", executablePath, PATH_MAX) == -1) {
    return {};
  }
  return dirname(executablePath);
#elif __WIN32__ || __WIN64__
  char executablePath[512];
  HMODULE hModule = GetModuleHandle(nullptr);
  if (hModule != nullptr) {
    // Use GetModuleFileName() with module handle to get the path
    GetModuleFileName(hModule, executablePath, (sizeof(executablePath)));
    PathRemoveFileSpec(executablePath);
    return executablePath;
  } else {
    return {};
  }
#endif
}

void forEachDirectory(const std::string &path, const DirConsumer &consumer) {
#if defined(__WIN32__) || defined(__WIN64__)
  WIN32_FIND_DATA fd;
  HANDLE hFind = FindFirstFile(path.c_str(), &fd);
  if (hFind != INVALID_HANDLE_VALUE) {
    do {
      // read all (real) files in current folder
      // , delete '!' read other 2 default folder . and ..
      if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
        consumer(fd.cFileName);
      }

    } while (FindNextFile(hFind, &fd));
    FindClose(hFind);
  } else {
    LOGE << "Could not open directory: " << path;
  }
#elif __linux__
  DIR *dir = nullptr;
  if ((dir = opendir(path.data())) != nullptr) {
    dirent *entry = nullptr;
    while ((entry = readdir(dir)) != nullptr) {
      if (entry->d_type == DT_DIR && strncmp(entry->d_name, "..", 2) != 0 &&
          strncmp(entry->d_name, ".", 1) != 0) {
        consumer(entry->d_name);
      }
    }
    closedir(dir);
  } else {
    LOGE << "Could not open directory: " << path;
  }
#endif
}

void forEachFile(const std::string &path, const DirConsumer &consumer) {
#if defined(__WIN32__) || defined(__WIN64__)
  WIN32_FIND_DATA fd;
  HANDLE hFind = FindFirstFile(path.c_str(), &fd);
  if (hFind != INVALID_HANDLE_VALUE) {
    do {
      if ((fd.dwFileAttributes & FILE_ATTRIBUTE_NORMAL) != 0) {
        consumer(fd.cFileName);
      }

    } while (FindNextFile(hFind, &fd));
    FindClose(hFind);
  } else {
    LOGE << "Could not open directory: " << path;
  }
#elif __linux__
  DIR *dir = nullptr;
  if ((dir = opendir(path.data())) != nullptr) {
    dirent *entry = nullptr;
    while ((entry = readdir(dir)) != nullptr) {
      if (entry->d_type == DT_REG) {
        consumer(entry->d_name);
      }
    }
    closedir(dir);
  } else {
    LOGE << "Could not open directory: " << path;
  }
#endif
}

std::time_t getLastModificationTime(const std::string &filePath) {
  struct stat statObject;
  if (stat(filePath.data(), &statObject) != 0) {
    LOGE << "Couldn't get file stat " << filePath;
    return 0;
  }
  return statObject.st_mtime;
}

char separator() {
#ifdef __linux__
  return '/';
#elif defined(__WIN32__) || defined(__WIN64__)
  return '\\';
#endif
}

}  // namespace tools
