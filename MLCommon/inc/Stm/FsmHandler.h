#ifndef FSMHANDLER_H
#define FSMHANDLER_H

#include <iostream>
#include <mutex>
#include <deque>
#include <utility>

#include "TransitionRow.h"

#include <plog/Log.h>

namespace ml {
namespace Stm {

template <class TStateMachine, class TState, class TEvents, class TParameter>
class FsmHandler
{

public:

    explicit FsmHandler(TStateMachine& fsmData);
    virtual ~FsmHandler();

    void fire_event(const typename TEvents::Enum ev, const TParameter* param);

    typename TState::Enum GetCurrentState() const;

    void SetCurrentState(const typename TState::Enum state);

    void Initialize(const typename TState::Enum state);

    void SetAdditionalLogging(const ::std::string& val);

private:

    bool PushAnyIfGuard(const TParameter* rescue);

    int GetNextEvent();
    void RunStateMachine();

    bool ProcessEvent(const int32_t ev, const TParameter* param);

    TStateMachine& _FsmData;
    typename TState::Enum _CurrentState;
    int _NumState;
    const TransitionRow<TStateMachine, TState, TEvents, TParameter>* _pTransitionTable;
    ::std::deque< ::std::pair < int32_t, const TParameter* > > _EventQueue;
    bool _IsPendingEvent;
    ::std::mutex _FsmMutex;

    bool _IsInitialized;

    ::std::string _additionalLogging;
};

template <class TStateMachine, class TState, class TEvents, class TParameter >
void FsmHandler<TStateMachine, TState, TEvents, TParameter>::Initialize(const typename TState::Enum state)
{
    SetCurrentState(state);
    _IsInitialized = true;
}

template < class TStateMachine, class TState, class TEvents, class TEvent >
void FsmHandler<TStateMachine, TState, TEvents, TEvent>::SetCurrentState(const typename TState::Enum state)
{
    _CurrentState = state;
}

template < class TStateMachine, class TState, class TEvents, class TParameter >
typename TState::Enum FsmHandler<TStateMachine, TState, TEvents, TParameter>::GetCurrentState() const
{
    return _CurrentState;
}

/////////////////////////////////
template < class TStateMachine, class TState, class TEvents, class TParameter>
FsmHandler<TStateMachine, TState, TEvents, TParameter>::FsmHandler(TStateMachine& fsmData) :
    _FsmData(fsmData),
    _CurrentState(),
    _NumState(fsmData.GetTransSize()),
    _pTransitionTable(TStateMachine::GetTrans()),
    _EventQueue(),
    _IsPendingEvent(false),
    _IsInitialized(false)
{
    /* Intentionally left blank */
}

template <class TStateMachine, class TState, class TEvents, class TParameter>
FsmHandler<TStateMachine, TState, TEvents, TParameter>::~FsmHandler()
{
    /* Intentionally left blank */
}

template <class TStateMachine, class TState, class TEvents, class TParameter>
void FsmHandler<TStateMachine, TState, TEvents, TParameter>::RunStateMachine()
{
    while (_EventQueue.size() > 0)
    {
        const typename TState::Enum prevState = _CurrentState;

        if (ProcessEvent(_EventQueue.front().first, _EventQueue.front().second))
        {
           LOGD << _additionalLogging << "transition from " << TState::ToString(prevState) << " to " << TState::ToString(_CurrentState) << " with event: " << (_EventQueue.front().first == ANY_EVENT ? "ANY_EVENT" : TEvents::ToString(static_cast<typename TEvents::Enum>(_EventQueue.front().first))) << ::std::endl;
        }
        else
        {
           LOGE << "No possible transition from " << TState::ToString(prevState) << " for the event " << (_EventQueue.front().first == ANY_EVENT ? "ANY_EVENT" : TEvents::ToString(static_cast<typename TEvents::Enum>(_EventQueue.front().first))) << ::std::endl;
        }

        const TParameter* rescue = _EventQueue.front().second;

        _EventQueue.pop_front();

        if (false == PushAnyIfGuard(rescue))
        {
            delete rescue;
        }
    }
}

template <class TStateMachine, class TState, class TEvents, class TParameter>
bool FsmHandler<TStateMachine, TState, TEvents, TParameter>::PushAnyIfGuard(const TParameter* rescue)
{
    bool isGuard = false;

    for (int i = 0; i < _NumState; i++)
    {
        if ((_CurrentState == _pTransitionTable[i]._StartState) && (_pTransitionTable[i]._Event == ANY_EVENT))
        {
            _EventQueue.push_front(std::make_pair(ANY_EVENT, rescue));
            isGuard = true;
            break;
        }
    }

    return isGuard;
}

template <class TStateMachine, class TState, class TEvents, class TParameter>
bool FsmHandler<TStateMachine, TState, TEvents, TParameter>::ProcessEvent(const int32_t ev, const TParameter* param)
{
    bool result = false;

    for (int i = 0; i < _NumState; i++)
    {
        if ((_CurrentState == _pTransitionTable[i]._StartState))
        {
            if (ev == _pTransitionTable[i]._Event)
            {
                int iHasConditionSatisfied = false;

                if (nullptr != _pTransitionTable[i].conditionfn)
                {
                    iHasConditionSatisfied = (_pTransitionTable[i].conditionfn(_FsmData, param));
                }

                if (iHasConditionSatisfied || (_pTransitionTable[i].conditionfn == NONE))
                {
                    _IsPendingEvent = true;

                    if (NULL != _pTransitionTable[i].fn)
                    {
                        _pTransitionTable[i].fn(_FsmData, param);
                    }

                    _IsPendingEvent = false;

                    _CurrentState = _pTransitionTable[i]._NextState;

                    result = true;

                    break;
                }
            }
        }
    }

    return result;
}

template <class TStateMachine, class TState, class TEvents, class TParameter>
void FsmHandler<TStateMachine, TState, TEvents, TParameter>::fire_event(const typename TEvents::Enum ev, const TParameter* param)
{
    if (true == _IsInitialized)
    {
        ::std::unique_lock< ::std::mutex > lk(_FsmMutex);
        _EventQueue.push_back(std::make_pair(ev, param));

        if (false == _IsPendingEvent)
        {
            RunStateMachine();
        }
    }
    else
    {
        LOGE << "Statemachine not initialized";
    }
}

template <class TStateMachine, class TState, class TEvents, class TParameter>
void FsmHandler<TStateMachine, TState, TEvents, TParameter>::SetAdditionalLogging(const ::std::string& val)
{
    _additionalLogging = val + " ";
}

} // namespace - Stm
} // namespace - ml

#endif // ! FSMHANDLER_H
