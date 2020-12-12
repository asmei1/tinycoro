//
// Created by Asmei on 11/27/2020.
//

#ifndef TINYCORO_IOCONTEXT_HPP
#define TINYCORO_IOCONTEXT_HPP
/*
 * 1. Create fd for whole process?
 * 2. Prepare MAX_EVENT
       If more than maxevents file descriptors are ready when epoll_wait()
       is called, then successive epoll_wait() calls will round robin
       through the set of ready file descriptors.  This behavior helps avoid
       starvation scenarios, where a process fails to notice that additional
       file descriptors are ready because it focuses on a set of file
       descriptors that are already known to be ready.
 * 3. Possibility to set timeout for wait for events
 *
 * //Cannot use EPOLLOUT for stdin (and i think that opposite way (stdout and EPOLLIN) it forbidden too...)
 * Maybe add some trait to get Event?
 * . Close fd
*/
#include <memory>
#include <sys/epoll.h>

#include "../Generator.hpp"
#include "IOEvent.hpp"

namespace tinycoro::io
{
    class IOContextException : public std::runtime_error
    {
    public:
        IOContextException(int errorCode, const char* msg) : std::runtime_error(msg)
        {}

        int errorCode = -1;
    };

    class IOContext
    {
    public:
        explicit IOContext(uint32_t eventPoolCount);
        ~IOContext();

        IOContext(IOContext&) = delete;
        IOContext& operator=(IOContext&) = delete;
        IOContext(IOContext&&) = delete;
        IOContext& operator=(IOContext&&) = delete;

        void processAwaitingEvents(int timeout);


        void scheduleEvent(IOEvent& event);
        void removeEvent(IOEvent& event);

        int epollFD = -1;
    private:
        friend class IOEvent;

        tinycoro::Generator<IOEvent::Coroutine> yieldAwaitingEvents(int timeout);

        std::unique_ptr<epoll_event[]> eventsList;
        const uint32_t eventPoolCount;
    };

};     // namespace tinycoro::io
#endif // TINYCORO_IOCONTEXT_HPP
