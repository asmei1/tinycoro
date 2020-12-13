//
// Created by Asmei on 11/27/2020.
//
#include "IOContext.hpp"
#include <unistd.h>

namespace tinycoro::io
{
    IOContext::IOContext(uint32_t eventPoolCount)
        : eventsList(std::make_unique<epoll_event[]>(eventPoolCount)),
          eventPoolCount(eventPoolCount)
    {
        this->epollFD = epoll_create1(EPOLL_CLOEXEC);

        if(-1 == this->epollFD)
        {
            throw std::system_error{errno, std::system_category(), strerror(errno)};
        }
    }

    IOContext::~IOContext()
    {
        close(this->epollFD);
    }

    void IOContext::processAwaitingEvents(int timeout)
    {
        for(auto& event : this->yieldAwaitingEvents(timeout))
        {
            event.resume();
        }
    }

    tinycoro::Generator<IOOperation::CoroHandle> IOContext::yieldAwaitingEvents(int timeout)
    {
        int eventCount = epoll_wait(this->epollFD, this->eventsList.get(), this->eventPoolCount, timeout);
        if(eventCount == -1)
        {
            throw std::system_error{errno, std::system_category(), strerror(errno)};
        }

        for(int i = 0; i < eventCount; ++i)
        {
            co_yield std::coroutine_handle<IOOperation>::from_address(this->eventsList[i].data.ptr);
        }
    }


} // namespace tinycoro::io