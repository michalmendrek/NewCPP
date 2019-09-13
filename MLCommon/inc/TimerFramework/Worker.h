#ifndef WORKER_H
#define WORKER_H

#include <atomic>
#include <condition_variable>
#include <list>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

#include "Event.h"
#include "Message.h"

#include "ITimerController.h"
#include "ITimerEntry.h"
#include "ITimerListener.h"
#include "TimerEvent.h"
#include "Timer.h"

namespace ml {

class Worker : public ITimerController
              ,public ITimerListener
{
    using MessageQueue = ::std::queue<Event*>;

    using TimerList = ::std::list<Timer*>;

public:

    Worker(const ::std::string& threadName);

    virtual ~Worker();

    bool Start();

    void Stop();

    void StopWithJoin();

    std::thread::id GetThreadId();

    virtual void Loop();

    void RequestStop();

    virtual void Run();

    void SendEvent(Event* event);

    // todo [chbe] why virtual?
    virtual void SendTimerEvent(TimerEvent* event);

    void SendMessage(Message* msg);

    void DefaultEventDispatcher();

    virtual void HandleMessage(const Message* msg);

    virtual bool StartTimer(const ITimerEntry& timerEntry, const Timer::TimeoutCallback_t& callback);

    virtual bool StopTimer(const ITimerEntry& timerEntry);

    Timer* GetTimer(const uint32_t id) const;

    bool AddTimer(Timer* t);

    bool RemoveTimer(const uint32_t timerID);

    virtual void HandleCallback(const uint32_t timerId);

protected:
    bool StopRequested() const;

private:

    void LogTimerList();

    Worker(const Worker&) = delete;
    Worker& operator=(const Worker&) = delete;

    bool StartTimer(Timer* timerId);
    bool StopTimer(uint32_t timerId);

    mutable ::std::mutex _StopMutex;
    bool _StopRequested;
    const ::std::string THREAD_NAME;
    ::std::thread* _Thread;
    MessageQueue _Queue;
    std::mutex _Mutex;
    std::condition_variable _Cv;
    TimerList _TimerList;
    std::atomic<bool> _TimerExit;
};

} // namespace - ml

#endif // ! WORKER_H
