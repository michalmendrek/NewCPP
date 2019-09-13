#ifndef TIMEREVENT_H
#define TIMEREVENT_H

#include <stdint.h>

#include "Event.h"

namespace ml {

class TimerEvent : public Event
{

public:

   explicit TimerEvent(const EventType::Enum eventType, const uint32_t timerId);

   virtual ~TimerEvent();

   inline uint32_t GetTimerId() const { return _TimerId; }

private:
   const uint32_t _TimerId;
};

} // namespace - ml

#endif // ! TIMEREVENT_H
