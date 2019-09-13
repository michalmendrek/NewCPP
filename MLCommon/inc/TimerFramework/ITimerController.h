#ifndef ITIMERCONTROLLER_H
#define ITIMERCONTROLLER_H

#include "ITimerEntry.h"
#include "Timer.h"

namespace ml {

class ITimerController
{

public:


   ITimerController() = default;

   virtual ~ITimerController() = default;

   virtual bool StartTimer(const ITimerEntry& timerEntry, const Timer::TimeoutCallback_t& callback) = 0;

   virtual bool StopTimer(const ITimerEntry& timerEntry) = 0;

private:

   ITimerController(const ITimerController&) = delete;

   ITimerController& operator =(const ITimerController&) = delete;

};

} // namespace - ml

#endif // ! ITIMERCONTROLLER_H
