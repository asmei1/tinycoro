//
// Created by Asmei on 12/2/2020.
//

#ifndef TINYCORO_IOEVENT_H
#define TINYCORO_IOEVENT_H
#include <coroutine>
#include <sys/epoll.h>
#include <utility>
#include "IOOperation.hpp"


namespace tinycoro::io
{
    class IOAsyncAutoResetEvent : public IOOperation
    {
    public:
        IOAsyncAutoResetEvent(IOContext& context);
        IOAsyncAutoResetEvent(IOContext& context, int fd);
        IOAsyncAutoResetEvent(IOAsyncAutoResetEvent& t) = delete;
        IOAsyncAutoResetEvent& operator=(IOAsyncAutoResetEvent&) = delete;

        IOAsyncAutoResetEvent& operator=(IOAsyncAutoResetEvent&& t) = delete;
        IOAsyncAutoResetEvent(IOAsyncAutoResetEvent&& t) = delete;

        virtual ~IOAsyncAutoResetEvent();

        bool await_ready() noexcept;
        void await_suspend(std::coroutine_handle<> awaitingCoro);
        void await_resume() noexcept;

        void set();
        bool isSignaled() const;
    protected:
        uint64_t extractFD() const;
    };

} // namespace tinycoro::io

#endif // TINYCORO_IOEVENT_H
