//
// Created by Asmei on 12/2/2020.
//

#ifndef TINYCORO_IOEVENT_H
#define TINYCORO_IOEVENT_H
#include <coroutine>
#include <utility>
#include <sys/epoll.h>

namespace tinycoro::io{
    class IOContext;

    class IOEvent
    {
    public:
        using Coroutine = std::coroutine_handle<>;

        IOEvent(IOContext& context);
        IOEvent(IOEvent& t) = delete;

        IOEvent(IOEvent&& t) = delete;

        ~IOEvent() = default;

        bool await_ready() noexcept;
        void await_suspend(std::coroutine_handle<> awaitingCoro);
        void await_resume() noexcept;

        epoll_event e;
    private:
        IOContext& ioContext;

    };
}

#endif // TINYCORO_IOEVENT_H
