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
    class IOEvent : public IOOperation
    {
    public:
        IOEvent(IOContext& context);
        IOEvent(IOContext& context, int fd);
        IOEvent(IOEvent& t) = delete;
        IOEvent& operator=(IOEvent&) = delete;

        IOEvent& operator=(IOEvent&& t) = delete;
        IOEvent(IOEvent&& t) = delete;

        virtual ~IOEvent();

        bool await_ready() noexcept;
        void await_suspend(std::coroutine_handle<> awaitingCoro);
        void await_resume() noexcept;

        size_t read(void* buffer, size_t buffSize);
        size_t write(void* buffer, size_t buffSize);

    protected:
    };

    class IOReadOnlyEvent : public IOEvent
    {
    public:
        IOReadOnlyEvent(IOContext& context);
        IOReadOnlyEvent(IOContext& context, int fd);

        size_t write(void* buffer, size_t buffSize) = delete;
    };

    class IOWriteOnlyEvent : public IOEvent
    {
    public:
        IOWriteOnlyEvent(IOContext& context);
        IOWriteOnlyEvent(IOContext& context, int fd);

        size_t read(void* buffer, size_t buffSize) = delete;
    };
} // namespace tinycoro::io

#endif // TINYCORO_IOEVENT_H
