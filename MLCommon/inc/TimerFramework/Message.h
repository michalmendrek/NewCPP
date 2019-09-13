#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdint.h>

#include "Event.h"

namespace ml {

class Message : public Event
{

public:

   explicit Message(const uint32_t msgId);

   virtual ~Message();

   inline uint32_t GetId() const { return _MsgId; }

private:
   const uint32_t _MsgId;
};

} // namespace - ml

#endif // ! MESSAGE_H
