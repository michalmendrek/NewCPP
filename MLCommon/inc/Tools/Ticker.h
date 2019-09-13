#ifndef TICKER_H
#define TICKER_H

#include <chrono>
#include <condition_variable>
#include <functional>
#include <thread>

class Ticker {
 public:
  using TimeoutFunction = std::function<void()>;
  Ticker() = default;
  Ticker(TimeoutFunction function, const bool singleShot = false)
      : mFunction{std::move(function)}, mSingleShot{singleShot} {}

  void SetFunction(const TimeoutFunction &function) {
    if (mRunning) {
      return;
    }
    mFunction = function;
  }

  void SetSingleShot(const bool singleShot) {
    if (mRunning) {
      return;
    }
    mSingleShot = singleShot;
  }
  bool IsSingleShot() const { return mSingleShot; }

  template <class Rep, class Period>
  void SetInterval(const std::chrono::duration<Rep, Period> &interval) {
    if (mRunning) {
      return;
    }
    mInterval = std::chrono::duration_cast<decltype(mInterval)>(interval);
  }

  void Start() {
    if (mRunning) {
      return;
    }
    if (!mFunction) {
      return;
    }
    if (mInterval == std::chrono::seconds::zero()) {
      return;
    }
    mRunning = true;
    mStopRequested = false;
    std::thread t([this]() {
      while (!mStopRequested) {
        std::unique_lock<std::mutex> lock(mMutex);
        mConditionVariable.wait_for(lock, mInterval,
                                    [this]() { return mStopRequested; });
        if (mStopRequested) {
          break;
        }
        mFunction();
        if (mSingleShot) {
          Stop();
          break;
        }
      }
      mRunning = false;
    });
    t.detach();
  }

  void Stop() {
    if (!mRunning) {
      return;
    }
    mStopRequested = true;
    mConditionVariable.notify_all();
  }

  bool IsRunning() const { return mRunning; }

 private:
  TimeoutFunction mFunction;
  std::chrono::seconds mInterval = std::chrono::seconds::zero();
  bool mSingleShot = false;
  std::condition_variable mConditionVariable;
  std::mutex mMutex;
  bool mStopRequested = false;
  bool mRunning = false;
};

#endif  // TICKER_H
