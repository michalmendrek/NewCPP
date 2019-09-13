#ifndef ITIMERLISTENER_H
#define ITIMERLISTENER_H

#include "TimerEvent.h"

namespace ml {

class ITimerListener
{

public:


   ITimerListener() = default;

   virtual ~ITimerListener() = default;

   virtual void SendTimerEvent(TimerEvent* event) = 0;

private:

   ITimerListener(const ITimerListener&) = delete;

   ITimerListener& operator =(const ITimerListener&) = delete;

};

} // namespace - ml

#endif // ! ITIMERLISTENER_H
