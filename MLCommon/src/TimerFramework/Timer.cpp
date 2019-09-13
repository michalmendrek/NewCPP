
#include "Timer.h"

#include "TimerEvent.h"

#include <plog/Log.h>

int ml::Timer::count = 0;

namespace ml {

Timer::Timer(const ITimerEntry& timerEntry, const TimeoutCallback_t& callback, ITimerListener& listener, const EventType::Enum type) :
   Event(type, timerEntry.getId()),
   _Cyclic(timerEntry.isCyclic()),
   _isRunning(false),
   _TimerCallback(callback),
   _Id(timerEntry.getId()),
   _Thread(NULL),
   _Interval(Interval(timerEntry.getDuration())),
   _TimerListener(listener)
{
    count++;

    LOGD << "Timer count = " << count;
    /* Intentionally left blank */
}

Timer::~Timer()
{
    if (nullptr != _Thread)
    {
        _Thread->join();
        delete _Thread;
        _Thread = nullptr;
    }

    count--;

    LOGD << "Destructor Timer count = " << count;
}

bool Timer::Start()
{
    if (_isRunning)
    {
        return false;
    }

    LOGD << "start timer with id = " << _Id;

    _isRunning = true;

    if (nullptr == _Thread)
    {
        _abortMutex.try_lock();
        _Thread = new ::std::thread(&Timer::Run, this);
    }

    return true;
}

void Timer::Stop()
{
    LOGD << "id = " << _Id;
    _isRunning = false;
    _abortMutex.unlock();

    if (nullptr != _Thread)
    {
        LOGD << "joining id = " << _Id;
        _Thread->join();
        delete _Thread;
        _Thread = nullptr;
    }
}

void Timer::RequestStop()
{
    Stop();
}

void Timer::Run()
{
    LOGD << "start sleep id = " << _Id << " interval = " << _Interval.count();
    LOGD << "awaken id = " << _Id;

    bool aborted = !_abortMutex.try_lock_for(_Interval);
    if (aborted)
    {
        LOGD << "is Running with _Id = " << _Id;
        TimerEvent* ev = new TimerEvent(EventType::TIMERCALLBACK, GetTimerID());
        _TimerListener.SendTimerEvent(ev);
    }
    else
    {
        LOGD << "not running with _Id = " << _Id;
    }
}

bool Timer::IsActive() const
{
    return _isRunning;
}

const uint32_t & Timer::GetTimerID() const
{
    return Event::GetEventId();
}

uint32_t Timer::GetInterval() const
{
    return _Interval.count();
}

void Timer::InvokeCallback()
{
    LOGD << "id = " << _Id;

    if (nullptr != _TimerCallback)
    {
        _TimerCallback();
    }
    else
    {
        LOGD << "_TimerCallback is null.";
    }
}

bool Timer::IsCyclic() const
{
    return _Cyclic;
}

} // namespace - ml
