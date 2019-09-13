
#include "Worker.h"

#include "TimerEvent.h"

#include <plog/Log.h>

using namespace std;

namespace ml {

Worker::Worker(const ::std::string& threadName) :
    _StopMutex(),
    _StopRequested(false),
    THREAD_NAME(threadName),
    _Thread(nullptr),
    _TimerExit(false)
{
    /* Intentionally left blank */
}

Worker::~Worker()
{
    Stop();
}

void Worker::SendEvent(Event* data)
{
    if (_Thread != nullptr)
    {
        std::unique_lock<std::mutex> lk(_Mutex);
        LOGD << "push event in worker queue";
        _Queue.push(data);
        lk.unlock();

        LOGD << "Wake up the worker";
        _Cv.notify_one();
    }
    else
    {
        printf("_Thread is null.\n");
    }
}

void Worker::SendMessage(Message* msg)
{
    SendEvent(dynamic_cast<Event*>(msg));
}

void Worker::DefaultEventDispatcher()
{
    while(!StopRequested())
    {
        // Wait for a message to be added to the queue
        std::unique_lock<std::mutex> lk(_Mutex);
        while (_Queue.empty())
        {
            LOGD << "Go sleep";
            _Cv.wait(lk);
        }

        LOGD << "awake";

        if (_Queue.empty())
        {
            continue;
        }

        Event* event = _Queue.front();
        _Queue.pop();

        lk.unlock();

        if (event != nullptr)
        {
            LOGD << "event = " << EventType::ToString(event->GetType());

            switch (event->GetType())
            {
                case (EventType::TIMERSTART) :
                {
                    Timer* timerStart = dynamic_cast< Timer*>(event);
                    StartTimer( timerStart );
                    break;
                }
                case (EventType::TIMERSTOP) :
                {
                    const TimerEvent* timerStop = dynamic_cast<const TimerEvent*>(event);
                    StopTimer( timerStop->GetTimerId() );
                    delete event;
                    break;
                }
                case (EventType::TIMERCALLBACK) :
                {
                    const TimerEvent* timerCallback = dynamic_cast<const TimerEvent*>(event);
                    HandleCallback( timerCallback->GetTimerId() );
                    delete event;
                    break;
                }
                case (EventType::MESSAGE) :
                {
                    const Message* msg = dynamic_cast<const Message*>(event);
                    HandleMessage( msg );
                    delete event;
                    break;
                }
                default:
                {
                    LOGE << "unknown event with type = " << event->GetType();
                }
            }
        }
    }
}

bool Worker::Start()
{
    bool result = false;

    _StopRequested = false;

    if (nullptr == _Thread)
    {
        _Thread = new thread(&Worker::Run, this);
        result = true;
    }

    return result;
}

std::thread::id Worker::GetThreadId()
{
    if (_Thread != nullptr)
    {
        return _Thread->get_id();
    }
    else
    {
        return std::thread::id();
    }
}

void Worker::Stop()
{
    RequestStop();
    if (!_Thread)
        return;

    _Thread->detach();
    delete _Thread;
    _Thread = nullptr;
}

void Worker::StopWithJoin()
{
    if (_Thread) {
        RequestStop();
        _Thread->join();
        delete _Thread;
        _Thread = nullptr;
    }
}

bool Worker::StopRequested() const
{
    std::unique_lock<std::mutex> lk(_StopMutex);
    return _StopRequested;
}

void Worker::RequestStop()
{
    std::unique_lock<std::mutex> lk(_StopMutex);
    _StopRequested = true;
}

void Worker::HandleMessage(const Message* msg)
{
    (void)(msg);

    LOGE << "Implement me";
}

void Worker::Run()
{
    while (!StopRequested())
    {
        Loop();
    }
}

void Worker::LogTimerList()
{
    LOGD << "timer count = " << _TimerList.size();

    TimerList::const_iterator cit(_TimerList.begin());

    if (cit != _TimerList.end())
    {
        for (; cit != _TimerList.end(); ++cit)
        {
            LOGD << "id = " <<  (*cit)->GetTimerID();
        }
    }
}

bool Worker::StartTimer(const ITimerEntry& timerEntry, const Timer::TimeoutCallback_t& callback)
{
    bool result = true;

    LOGD << "try start timer";
    Event* evt = new Timer(timerEntry, callback, *this, EventType::TIMERSTART);
    SendEvent(evt);

    return result;
}

bool Worker::StopTimer(const ITimerEntry& timerEntry)
{
    bool result = true;

    LOGD << "Send Stop Timer Event";
    Event* evt = new TimerEvent(EventType::TIMERSTOP, timerEntry.getId());
    SendEvent(evt);

    return result;
}

Timer* Worker::GetTimer(const uint32_t id) const
{
    Timer* timer = nullptr;
    std::list<Timer*>::const_iterator it;

    for(it = _TimerList.begin(); it != _TimerList.end(); ++it)
    {
        if ((*it)->GetTimerID() == id)
        {
            timer = *it;
            break;
        }
    }

    return timer;
}

bool Worker::AddTimer(Timer* timer)
{
    bool result = false;

    if (nullptr != timer)
    {
        if (0 != timer->GetTimerID())
        {
            if (nullptr == GetTimer(timer->GetTimerID()))
            {
                _TimerList.push_back(timer);
                result = true;
            }
        }
    }

    return result;
}

bool Worker::RemoveTimer(const uint32_t timerID)
{
    bool result = false;

    LOGD << "timerID = " << timerID;

    std::list<Timer*>::iterator it;

    for (it = _TimerList.begin(); it != _TimerList.end(); ++it)
    {
        if ((*it)->GetTimerID() == timerID)
        {
            LOGD << "Remove timer with id = " << timerID;
            _TimerList.erase(it);
            result = true;
            break;
        }
    }

    return result;
}

bool Worker::StartTimer(Timer* timer)
{
    bool result = false;

    if (nullptr != GetTimer(timer->GetTimerID()))
    {
        StopTimer(timer->GetTimerID());
    }

    if (nullptr != timer)
    {
        AddTimer(timer);
        timer->Start();
    }

    return result;
}

bool Worker::StopTimer(const uint32_t timerId)
{
    bool result = false;

    Timer* timer = GetTimer(timerId);

    if (nullptr != timer)
    {
        LOGD << "Request Stop Timer";
        timer->RequestStop();
        result = RemoveTimer(timerId);
    }

    return result;
}

void Worker::HandleCallback(const uint32_t timerId)
{
    Timer* timer = GetTimer(timerId);

    if (nullptr != timer)
    {
        timer->InvokeCallback();

        if (false == timer->IsCyclic())
        {
            StopTimer(timerId);
        }

        if (true == timer->IsCyclic())
        {
            timer->Stop();
            timer->Start();
        }
    }
}

void Worker::SendTimerEvent(TimerEvent* event)
{
    SendEvent(event);
}

void Worker::Loop()
{
    DefaultEventDispatcher();
}

} // namespace - ml
