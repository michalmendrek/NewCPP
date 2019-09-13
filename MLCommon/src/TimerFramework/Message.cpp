
#include "Message.h"

namespace ml {

Message::Message(const uint32_t msgId) :
   Event(ml::EventType::MESSAGE, msgId),
   _MsgId(msgId)
{
   /* Intentionally left blank. */
}

Message::~Message()
{
   /* Intentionally left blank. */
}

} // namespace - ml
