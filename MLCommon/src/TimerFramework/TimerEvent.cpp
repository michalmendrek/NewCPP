
#include "TimerEvent.h"

namespace ml {

TimerEvent::TimerEvent(const EventType::Enum eventType, const uint32_t timerId) :
   Event(eventType, timerId),
   _TimerId(timerId)
{
   /* Intentionally left blank. */
}

TimerEvent::~TimerEvent()
{
   /* Intentionally left blank. */
}

} // namespace - ml
