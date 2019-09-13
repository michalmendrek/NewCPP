#ifndef EPOLL_WORKER_H
#define EPOLL_WORKER_H

#include <plog/Log.h>
#include <sys/epoll.h>
#include <functional>
#include <vector>

namespace ml {

template <int EPOLL_MAX_EVENTS, int EPOLL_WAIT_MS>
class EPollWorker {
 public:
  struct EpollItem {
    unsigned int events;
    int fileDescriptor;
    void* ptr;
  };
  using DescrProvider = std::function<const EpollItem(const size_t index)>;
  using EPollHandler = std::function<void(const epoll_event& event)>;
  using EPollErrorHandler = std::function<void(const int errorCode)>;

  ~EPollWorker() { cleanupEPoll(); }

  bool prepareEPoll() {
    if (pollingfd < 0) {
      pollingfd = epoll_create(1);  // see epoll_create Notes
      if (pollingfd < 0) {
        LOGE << "Can't create epoll";
        return false;
      }
    }
    return true;
  }

  bool prepareEPoll(const size_t count, const DescrProvider& prv) {
    if (pollingfd < 0) {
      pollingfd = epoll_create(static_cast<int>(count));
      if (pollingfd < 0) {
        LOGE << "Can't create epoll";
        return false;
      }
      for (size_t t = 0; t < count; t++) {
        const EpollItem& item = prv(t);
        if (not addToEPoll(item.fileDescriptor, item.ptr, item.events)) {
          return false;
        }
      }
    }
    return true;
  }

  void cleanupEPoll() {
    close(pollingfd);
    pollingfd = -1;
  }

  void waitEPoll(const EPollHandler& handler,
                 const EPollErrorHandler& errorHandler) {
    if (pollingfd >= 0) {
      epoll_event pevents[EPOLL_MAX_EVENTS];

      const int ready =
          epoll_wait(pollingfd, pevents, EPOLL_MAX_EVENTS, EPOLL_WAIT_MS);
      if (ready < 0) {
        errorHandler(ready);

      } else {
        for (int t = 0; t < ready; t++) {
          handler(pevents[t]);
        }
      }
    }
  }

  bool addToEPoll(const int fileDescr, const unsigned int events) {
    if (pollingfd >= 0) {
      epoll_event ev{};
      ev.events = events;
      ev.data.fd = fileDescr;
      if (epoll_ctl(pollingfd, EPOLL_CTL_ADD, fileDescr, &ev) == 0) {
        return true;
      }
      LOGE << "Can't add file descriptor " << fileDescr << " to epoll";
    }
    return false;
  }

  bool addToEPoll(const int fileDescr, void* ptr, const unsigned int events) {
    if (pollingfd >= 0) {
      epoll_event ev{};
      ev.events = events;
      ev.data.ptr = ptr;
      if (epoll_ctl(pollingfd, EPOLL_CTL_ADD, fileDescr, &ev) == 0) {
        return true;
      }
      LOGE << "Can't add file descriptor " << fileDescr << " to epoll";
    }
    return false;
  }

  bool removeFromEPoll(const int fileDescr) {
    if (pollingfd >= 0) {
      if (epoll_ctl(pollingfd, EPOLL_CTL_DEL, fileDescr, nullptr) == 0) {
        return true;
      }
      LOGE << "Can't remove file descriptor " << fileDescr << " from epoll";
    }
    return false;
  }

 private:
  int pollingfd{-1};
};

}  // namespace ml
#endif  // !EPOLL_WORKER
