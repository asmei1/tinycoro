//
// Created by Asmei on 11/27/2020.
//
#include "IOContext.hpp"
#include <cstring>
#include <iostream>
#include <sys/epoll.h>
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
            throw IOContextException{errno, strerror(errno)};
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

    tinycoro::Generator<IOEvent::Coroutine> IOContext::yieldAwaitingEvents(int timeout)
    {
        int eventCount = epoll_wait(this->epollFD, this->eventsList.get(), this->eventPoolCount, timeout);

        if(eventCount == -1)
        {
            throw IOContextException(errno, strerror(errno));
        }
        for(int i = 0; i < eventCount; ++i)
        {
            co_yield std::coroutine_handle<IOEvent>::from_address(this->eventsList[i].data.ptr);
        }
    }

    void IOContext::scheduleEvent(IOEvent& event)
    {
        // ok?
        if(0 == epoll_ctl(this->epollFD, EPOLL_CTL_ADD, event.fd, &event.eventSettings))
            return;

        if(errno == EEXIST)
            // re-init event
            if(0 == epoll_ctl(this->epollFD, EPOLL_CTL_MOD, event.fd, &event.eventSettings))
                return;

        // if there were some errors during event polling, throw exception
        throw IOContextException(errno, strerror(errno));
    }

    void IOContext::removeEvent(IOEvent& event)
    {
        if(0 != epoll_ctl(this->epollFD, EPOLL_CTL_DEL, event.fd, &event.eventSettings))
            throw IOContextException{errno, "epoll_ctl EPOLL_CTL_DEL"};
    }

} // namespace tinycoro::io