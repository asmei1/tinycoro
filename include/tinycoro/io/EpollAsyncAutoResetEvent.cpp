//
// Created by Asmei on 12/2/2020.
//

#include "EpollAsyncAutoResetEvent.hpp"
#include "IOContext.hpp"
#include <sys/eventfd.h>
#include <unistd.h>
namespace tinycoro::io
{
    EpollAsyncAutoResetEvent::EpollAsyncAutoResetEvent(IOContext& context) : IOOperation(context)
    {
        this->fd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
        //I have to think if epolloneshot is required - without it set call is valid only if object is co_awaited
        this->eventData.events = EPOLLET | EPOLLIN;//| EPOLLONESHOT;
    }

    EpollAsyncAutoResetEvent::~EpollAsyncAutoResetEvent()
    {
        if(this->fd != 0)
            close(this->fd);
    }

    bool EpollAsyncAutoResetEvent::await_ready() noexcept
    {
        return false;
    }

    void EpollAsyncAutoResetEvent::await_suspend(std::coroutine_handle<> awaitingCoro)
    {
        this->eventData.data.ptr = awaitingCoro.address();

        this->ioContext.scheduleOperation(*this);
    }

    void EpollAsyncAutoResetEvent::await_resume()
    {
        this->throwExceptionIfCanceled();
        uint64_t temp;
        if(-1 == read(this->fd, &temp, sizeof(temp)))
            throw std::system_error{errno, std::system_category(), strerror(errno)};
    }

    void EpollAsyncAutoResetEvent::set()
    {
        // Don't care about value passed to write - it's called only for invoke epoll and to put this event to active fd
        // list
        if(-1 == write(this->fd, &this->fd, sizeof(this->fd)))
            throw std::system_error{errno, std::system_category(), strerror(errno)};
    }

} // namespace tinycoro::io