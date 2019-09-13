#ifndef TIMERENTRY_H
#define TIMERENTRY_H

#include "ITimerEntry.h"
#include "Timer.h"

namespace ml {

template<const int64_t TimeoutMs, const uint32_t Id, const bool Cyclic = false, const bool Autostart = false>
class TimerEntry : public ITimerEntry
{

public:

   explicit TimerEntry();
   virtual ~TimerEntry();

   bool operator==(const TimerEntry& entry) const;

   /// Get the duration for the Timer (ms).
   virtual int64_t getDuration() const;

   /// Get the Id for the timer.
   virtual uint32_t getId() const;

   /// Get the information if cyclic or not
   virtual bool isCyclic() const;

   /// Get the information if autostart or not
   virtual bool isAutostart() const;
};

template<const int64_t TimeoutMs, const uint32_t Id, const bool Cyclic, const bool Autostart>
TimerEntry<TimeoutMs, Id, Cyclic, Autostart>::TimerEntry()
{
   /* Intentionally left blank */
}

template<const int64_t TimeoutMs, const uint32_t Id, const bool Cyclic, const bool Autostart>
TimerEntry<TimeoutMs, Id, Cyclic, Autostart>::~TimerEntry()
{
   /* Intentionally left blank */
}

template<const int64_t TimeoutMs, const uint32_t Id, const bool Cyclic, const bool Autostart>
bool TimerEntry<TimeoutMs, Id, Cyclic, Autostart>::operator==(const TimerEntry& entry) const
{
   return (entry.getId() == getId());
}

template<const int64_t TimeoutMs, const uint32_t Id, const bool Cyclic, const bool Autostart>
int64_t TimerEntry<TimeoutMs, Id, Cyclic, Autostart>::getDuration() const
{
   return TimeoutMs;
}

template<const int64_t TimeoutMs, const uint32_t Id, const bool Cyclic, const bool Autostart>
uint32_t TimerEntry<TimeoutMs, Id, Cyclic, Autostart>::getId() const
{
   return Id;
}

template<const int64_t TimeoutMs, const uint32_t Id, const bool Cyclic, const bool Autostart>
bool TimerEntry<TimeoutMs, Id, Cyclic, Autostart>::isCyclic() const
{
   return Cyclic;
}

template<const int64_t TimeoutMs, const uint32_t Id, const bool Cyclic, const bool Autostart>
bool TimerEntry<TimeoutMs, Id, Cyclic, Autostart>::isAutostart() const
{
   return Autostart;
}

} // namespace - ml

#endif // ! TIMERENTRY_H
