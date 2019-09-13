#ifndef TIMINGS_H
#define TIMINGS_H

#include "TimerFramework/TimerEntry.h"

class Timings
{
public:
    typedef ml::TimerEntry<1000, 1> WaitForReconnect;

private:
    explicit Timings();
    Timings(const Timings&);
    Timings& operator=(const Timings&);
};

#endif // ! TIMINGS_H
