#ifndef TIMER_H
#define TIMER_H

#include <chrono>

#include <functional>

#include <thread>
#include <mutex>

#include "Event.h"

#include "ITimerEntry.h"

#include "ITimerListener.h"

namespace ml {

class Timer : public Event
{

public:

    static int count;

    static const uint32_t START_ID_CALLBACKTIMER = 1000;

    typedef ::std::chrono::milliseconds Interval;
    typedef ::std::function<void(void)> TimeoutCallback_t;

    explicit Timer(const ITimerEntry& timerEntry, const TimeoutCallback_t& callback, ITimerListener& listener, const EventType::Enum type);

    virtual ~Timer();

    bool Start();

    void RequestStop();

    void Stop();

    const uint32_t & GetTimerID() const;

    bool IsCyclic() const;

    void InvokeCallback();

    bool IsActive() const;

private:

    uint32_t GetInterval() const;

    void Run();

    void Init();

    bool _Cyclic;
    bool _isRunning;
    bool _IsAutostart;

    const TimeoutCallback_t _TimerCallback;

    const uint32_t _Id;
    ::std::thread* _Thread;
    Interval _Interval;
    std::timed_mutex _abortMutex;
    ITimerListener& _TimerListener;
};

} // namespace - ml

#endif // ! TIMER_H
