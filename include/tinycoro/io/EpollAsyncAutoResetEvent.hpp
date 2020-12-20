//
// Created by Asmei on 12/2/2020.
//

#ifndef TINYCORO_IOEVENT_H
#define TINYCORO_IOEVENT_H
#include <coroutine>
#include <utility>
#include "IOOperation.hpp"


namespace tinycoro::io
{
    /*
     * A simple class with set and auto clear mechanism, that allow to wait one thread
     * until an event is signalled by a thread calling a set() function.
     *
     * State set is automatically set to "not set" state after wake up thread.
     */
    class EpollAsyncAutoResetEvent : public IOOperation
    {
    public:
        EpollAsyncAutoResetEvent(IOContext& context);

        ~EpollAsyncAutoResetEvent();

        bool await_ready() noexcept;
        void await_suspend(std::coroutine_handle<> awaitingCoro);
        void await_resume();

        void set();
    };

} // namespace tinycoro::io

#endif // TINYCORO_IOEVENT_H
