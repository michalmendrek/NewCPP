
#include "Event.h"

namespace ml {

Event::Event(const ml::EventType::Enum type, const EventId id) :
        _Type(type),
        _Id(id)
{
	/* Intentionally left blank */
}

Event::~Event()
{
    /* Intentionally left blank */
}

const ml::Event::EventId & Event::GetEventId() const
{
    return _Id;
}

const EventType::Enum & Event::GetType() const
{
    return _Type;
}

} // namespace - ml
