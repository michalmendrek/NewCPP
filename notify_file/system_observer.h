#ifndef SYSTEM_OBSERVER_H_A322432
#define SYSTEM_OBSERVER_H_A322432

#include "EPoolWorker.h"
#include <functional>
#include "folder_monitor.h"
#include "system_observable.h"

static constexpr int SYSTEM_OBSERVER_EPOOL_EVENTS = 4;
static constexpr int SYSTEM_OBSERVER_EPOOL_WAIT = 1 * 1000;

class SystemObserver : public ml::EPollWorker<SYSTEM_OBSERVER_EPOOL_EVENTS,
                                              SYSTEM_OBSERVER_EPOOL_WAIT> {
 public:
  using ShouldContinue = std::function<bool()>;

  SystemObserver() {}

  ~SystemObserver() { cleanupEPoll(); }

  void addObserver(SystemObservable&& observer) {
    observers.emplace_back(std::move(observer));
  }

  void monitor(ShouldContinue shouldRun) {
    cleanupEPoll();
    prepareEPoll();
    for (const SystemObservable& obs : observers) {
      addToEPoll(obs.getFileDescriptor(), EPOLLIN | EPOLLET);
    }

    while (shouldRun()) {
      waitEPoll(
          [=](const epoll_event& event) {
            for (SystemObservable& obs : observers) {
              if (event.data.fd == obs.getFileDescriptor()) {
                obs.handleIncommingEvents();
              }
            }
          },

          [](const int errorCode) {
            //LOGE << "Error on epoll, error code:" << errorCode;
          });
      for (SystemObservable& obs : observers) {
        obs.process();
      }
    }
  }

 private:
  std::vector<SystemObservable> observers;
};

#endif  // ! SYSTEM_OBSERVER_H_A322432
