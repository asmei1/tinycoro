//
// Created by Asmei on 12/2/2020.
//

#include "IOEvent.hpp"
#include "IOContext.hpp"
#include <sys/eventfd.h>
#include <unistd.h>
#include <fcntl.h>

namespace tinycoro::io
{
    IOEvent::IOEvent(IOContext& context) : IOOperation(context)
    {
        this->settings.events = EPOLLIN | EPOLLOUT | EPOLLONESHOT;
        this->fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    }

    IOEvent::IOEvent(IOContext& context, int fd) : IOOperation(context)
    {
        this->fd = fd;
        this->settings.events = EPOLLIN | EPOLLOUT | EPOLLONESHOT;

        if(not (fcntl(fd, F_GETFD) != -1 || errno != EBADF)){
            throw std::system_error{errno, std::system_category(), "Passed invalid file description"};
        }
    }

    IOEvent::~IOEvent()
    {
        close(this->fd);
    }

    bool IOEvent::await_ready() noexcept
    {
        return false;
    }

    void IOEvent::await_suspend(std::coroutine_handle<> awaitingCoro)
    {
        this->settings.data.ptr = awaitingCoro.address();

        this->ioContext.scheduleOperation(*this);
    }

    void IOEvent::await_resume() noexcept
    {}

    size_t IOEvent::write(void* buffer, size_t buffSize)
    {
        return ::write(this->fd, buffer, buffSize);
    }

    size_t IOEvent::read(void* buffer, size_t buffSize)
    {
        return ::read(this->fd, buffer, buffSize);
    }

    IOReadOnlyEvent::IOReadOnlyEvent(IOContext& context) : IOEvent(context)
    {
        this->settings.events = EPOLLIN | EPOLLONESHOT;
    }

    IOReadOnlyEvent::IOReadOnlyEvent(IOContext& context, int fd) : IOEvent(context, fd)
    {
        this->settings.events = EPOLLIN | EPOLLONESHOT;
    }

    IOWriteOnlyEvent::IOWriteOnlyEvent(IOContext& context) : IOEvent(context)
    {
        this->settings.events = EPOLLOUT | EPOLLONESHOT;
    }

    IOWriteOnlyEvent::IOWriteOnlyEvent(IOContext& context, int fd) : IOEvent(context, fd)
    {
        this->settings.events = EPOLLOUT | EPOLLONESHOT;
    }
} // namespace tinycoro::io