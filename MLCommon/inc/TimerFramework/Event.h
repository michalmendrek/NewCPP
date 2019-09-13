#ifndef EVENT_H
#define EVENT_H

#include <stdint.h>

#include "EventType.h"

namespace ml {


class Event {

public:

    using EventId = uint32_t;

    Event(const EventType::Enum type, const EventId id);

	virtual ~Event();

    const EventId & GetEventId() const;
    const EventType::Enum & GetType() const;

    EventType::Enum _Type;
    EventId _Id;
};

} // namespace - ml

#endif // ! EVENT_H
