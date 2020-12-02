//
// Created by Asmei on 12/2/2020.
//

#include "IOEvent.hpp"
#include "IOContext.hpp"

namespace tinycoro::io
{
    IOEvent::IOEvent(IOContext& context)
    :ioContext(context)
    {
    }

    bool IOEvent::await_ready() noexcept
    {
        return false;
    }
bool init = false;
    void IOEvent::await_suspend(std::coroutine_handle<> awaitingCoro)
    {
        if(not init)
        {
            //it should goes to IOContext eventList
            epoll_event event;

            constexpr int stdinFD = 0;

            event.events = EPOLLIN;
            event.data.fd = 0;
            event.data.ptr = awaitingCoro.address();
            if(-1 == epoll_ctl(ioContext.epollFD, EPOLL_CTL_ADD, stdinFD, &event))
            {
                exit(1);
            }
            init =true;
        }
    }

    void IOEvent::await_resume() noexcept
    {

    }

}