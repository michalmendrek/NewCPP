#ifndef FOLDER_MONITOR_H_88123A
#define FOLDER_MONITOR_H_88123A

#include <plog/Log.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <chrono>
#include <functional>
#include <ios>
#include <string>
#include <vector>

class FolderMonitor {
 public:
  using ProcessConsumer = std::function<void(const std::string path)>;

  explicit FolderMonitor(const std::string& path)
      : descriptor(inotify_init1(IN_NONBLOCK)), eventBuffer(EVENT_BUFF_SIZE) {
    if (descriptor < 0) {
      throw std::ios_base::failure("inotify_init1() failed");
    }
    int wd = inotify_add_watch(descriptor, path.c_str(),
                               IN_CREATE | IN_CLOSE | IN_OPEN | IN_DELETE);
    if (wd < 0) {
      close(descriptor);
      throw std::ios_base::failure("inotify_add_watch() failed");
    }
    LOGD << "Started observation of folder: " << path
         << ", handle:" << descriptor;
  }

  ~FolderMonitor() {
    if (descriptor >= 0) {
      close(descriptor);
      LOGD << "Finished observation of folder with handle: " << descriptor;
    }
  }

  FolderMonitor(const FolderMonitor& other) = delete;
  FolderMonitor operator=(const FolderMonitor& other) = delete;

  FolderMonitor(FolderMonitor&& other)
      : watchedFiles(std::move(other.watchedFiles)) {
    std::swap(descriptor, other.descriptor);
    std::swap(fileReadyTimeout, other.fileReadyTimeout);
  }

  FolderMonitor& operator=(FolderMonitor&& other) {
    watchedFiles = std::move(other.watchedFiles);
    std::swap(descriptor, other.descriptor);
    std::swap(fileReadyTimeout, other.fileReadyTimeout);
    return *this;
  }

  int getFileDescriptor() const { return descriptor; }

  void handleIncommingEvents() {
    ssize_t length = read(descriptor, eventBuffer.data(), eventBuffer.size());
    if (length > 0) {
      parseEvents(length);
    }
  }

  void process(ProcessConsumer consumer) {
    for (WatchedFile& file : watchedFiles) {
      if ((not file.consumed) and (not file.isInUse()) and
          file.notTouchedIn(fileReadyTimeout)) {
        file.consumed = true;
        consumer(file.getPath());
      }
    }
  }

  void setFileReadyTimeout(std::chrono::milliseconds timeout) {
    fileReadyTimeout = timeout;
  }

 private:
  class WatchedFile {
   public:
    bool consumed{false};

    explicit WatchedFile(const std::string& path) : path(path) { touched(); }

    WatchedFile(WatchedFile&& other) = default;
    WatchedFile& operator=(WatchedFile&& other) = default;

    bool notTouchedIn(std::chrono::milliseconds unusedTime) const {
      return std::chrono::duration_cast<std::chrono::milliseconds>(
                 std::chrono::steady_clock::now() - lastAccessTime) >
             unusedTime;
    }

    bool isInUse() const { return inUse; }

    void handleStateChange(uint32_t event) {
      touched();
      if (event & IN_OPEN) {
        inUse = true;

      } else if (event & IN_CLOSE) {
        inUse = false;
      }
    }

    const std::string& getPath() const { return path; }

   private:
    std::chrono::steady_clock::time_point lastAccessTime;
    bool inUse{false};
    std::string path;

    void touched() { lastAccessTime = std::chrono::steady_clock::now(); }
  };

  static constexpr int EVENT_BUFF_SIZE = sizeof(inotify_event) * 4;
  static constexpr std::chrono::milliseconds FILE_READY_TIMEOUT =
      std::chrono::milliseconds(2 * 1000);
  using WatchFileVector = std::vector<WatchedFile>;
  int descriptor{-1};
  std::chrono::milliseconds fileReadyTimeout{FILE_READY_TIMEOUT};
  std::vector<uint8_t> eventBuffer;
  WatchFileVector watchedFiles;

  WatchFileVector::iterator getWatchedFileIterator(const std::string& path) {
    return std::find_if(
        watchedFiles.begin(), watchedFiles.end(),
        [&](const WatchedFile& f) { return f.getPath() == path; });
  }

  void parseEvents(ssize_t length) {
    ssize_t index{0};
    while (index < length) {
      inotify_event* event =
          reinterpret_cast<struct inotify_event*>(eventBuffer.data() + index);
      index += sizeof(inotify_event) + event->len;

      if (event->mask & IN_IGNORED) {
        continue;
      }

      std::string path{event->name, event->len};
      if (event->mask & IN_CREATE) {
        LOGI << "New file to observe at:" << path;
        watchedFiles.emplace_back(path);

      } else if (event->mask & IN_DELETE) {
        auto it = getWatchedFileIterator(path);
        if (it != watchedFiles.end()) {
          watchedFiles.erase(it);
          LOGI << "Finished observation of file:" << path;
        }

      } else {
        auto it = getWatchedFileIterator(path);
        if (it != watchedFiles.end()) {
          (*it).handleStateChange(event->mask);
          LOGI << "Change to observated file:" << path;
        }
      }
    }
  }
};

#endif  // ! FOLDER_MONITOR_H_88123A
