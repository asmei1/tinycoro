//
// Created by Asmei on 12/2/2020.
//

#include "IOAsyncAutoResetEvent.hpp"
#include "IOContext.hpp"
#include <sys/eventfd.h>
#include <unistd.h>

constexpr auto eventMask = 1ULL << 63;

namespace tinycoro::io
{
    IOAsyncAutoResetEvent::IOAsyncAutoResetEvent(IOContext& context) : IOOperation(context)
    {
        this->fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
        this->settings.events = EPOLLIN | EPOLLOUT | EPOLLONESHOT;
    }

    IOAsyncAutoResetEvent::~IOAsyncAutoResetEvent()
    {
        close(this->fd);
    }

    bool IOAsyncAutoResetEvent::await_ready() noexcept
    {
        return this->isSignaled();
    }

    void IOAsyncAutoResetEvent::await_suspend(std::coroutine_handle<> awaitingCoro)
    {
        this->settings.data.ptr = awaitingCoro.address();

        this->ioContext.scheduleOperation(*this);
    }

    void IOAsyncAutoResetEvent::await_resume() noexcept
    {
        this->fd = this->extractFD();
    }

    void IOAsyncAutoResetEvent::set()
    {
        if(this->isSignaled())
            return; // there race condition (event is signaled, so this check is risky!

        // Don't care about value passed to write - it's called only for invoke epoll and to put this event to active fd
        // list
        if(-1 == write(this->fd, &this->fd, sizeof(this->fd)))
            throw std::system_error{errno, std::system_category(), strerror(errno)};

        this->fd = eventMask | this->fd;
    }

    bool IOAsyncAutoResetEvent::isSignaled() const
    {
        return this->fd & eventMask;
    }
    uint64_t IOAsyncAutoResetEvent::extractFD() const
    {
        return ~eventMask & this->fd;
    }

} // namespace tinycoro::io