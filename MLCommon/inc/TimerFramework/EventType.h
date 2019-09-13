#ifndef EVENTTYPE_H
#define EVENTTYPE_H

#include <string>

namespace ml {

class EventType
{

public:
    enum Enum
    {
        TIMERCALLBACK = 0,
        TIMERSTART = 1,
        TIMERSTOP = 2,
        MESSAGE = 3
    };

    static ::std::string ToString(const Enum& e)
    {
        ::std::string res;

        switch (e)
        {
            case (TIMERCALLBACK) : res = "TIMERCALLBACK"; break;
            case (TIMERSTART)    : res = "TIMERSTART"; break;
            case (TIMERSTOP)     : res = "TIMERSTOP"; break;
            case (MESSAGE)       : res = "MESSAGE"; break;

            // No default case since we want to get a warning
        }

        return res;
    }

private:

    EventType() = delete;
    EventType(const EventType& o) = delete;
    EventType& operator=(const EventType& o) = delete;

};

} // namespace - ml

#endif // ! EVENTTYPE_H
