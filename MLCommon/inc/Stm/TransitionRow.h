#ifndef TRANSITIONROW_H
#define TRANSITIONROW_H

#include <stdint.h>

#define NONE nullptr

static const int32_t ANY_EVENT = -1;

namespace ml {
namespace Stm {

template < class TStatemachine, class TState, class TEvents, class TParameter>
struct TransitionRow
{
    typename TState::Enum _StartState;
    int32_t _Event;
    void (*fn) (TStatemachine&, const TParameter*);
    typename TState::Enum _NextState;
    bool (*conditionfn) (TStatemachine&, const TParameter*);
};

} // namespace - Stm
} // namespace - ml

#endif // ! TRANSITIONROW_H
