#ifndef ITIMERENTRY_H
#define ITIMERENTRY_H

#include <stdint.h>

namespace ml {

class ITimerEntry
{

public:

   ITimerEntry() = default;

   virtual ~ITimerEntry() = default;

   virtual int64_t getDuration() const = 0;

   virtual uint32_t getId() const = 0;

   virtual bool isCyclic() const = 0;

   virtual bool isAutostart() const = 0;

private:

   ITimerEntry(const ITimerEntry&) = delete;

   ITimerEntry& operator =(const ITimerEntry&) = delete;

};

inline bool operator==(const ITimerEntry& entry1, const ITimerEntry& entry2) noexcept {
  return (entry1.getId() == entry2.getId());
}

inline bool operator!=(const ITimerEntry& entry1, const ITimerEntry& entry2) noexcept {
  return !(entry1 == entry2);
}

} // namespace - ml

#endif // ! ITIMERENTRY_H
