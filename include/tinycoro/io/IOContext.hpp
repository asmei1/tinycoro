//
// Created by Asmei on 11/27/2020.
//

#ifndef TINYCORO_IOCONTEXT_HPP
#define TINYCORO_IOCONTEXT_HPP

#include <chrono>
#include <cstring>
#include <memory>
#include <sys/epoll.h>

#include "../Generator.hpp"
#include "IOOperation.hpp"

namespace tinycoro::io
{
    /*
     * IOContext is the class for scheduling and invoking IO events, based on epoll_event mechanism from Linux.
     */
    class IOContext
    {
    public:
        /*
         * Create IOContext object.
         * @param eventPoolCount number of events which will be processed in one call of epoll_wait.
         */
        explicit IOContext(uint32_t eventPoolCount);
        ~IOContext();

        // Non-copyable
        IOContext(IOContext&) = delete;
        IOContext& operator=(IOContext&) = delete;

        /*
         * Wait for awaiting IO events and resume them.
         * Blocks current thread until event will ready (ie. file descriptor deliver an event)
         *
         * @throw std::system_error Throw std::system_error when something went wrong.
         */
        void processAwaitingEvents();

        /*
         * Wait for awaiting IO events and resume them.
         * Blocks current thread until event will ready (ie. file descriptor deliver an event)
         * or timeout expires
         *
         * @param timeout Specifies timeout to wait.
         * @throw std::system_error Throw std::system_error when something went wrong.
         */
        void processAwaitingEvents(const std::chrono::milliseconds& timeout);

        /*
         * Pool any derived from IOOperation object.
         *
         * @param operation Any object, which derivered from IOOperation class.
         * @throw std::system_error Throw std::system_error when something went wrong.
         */
        void scheduleOperation(std::derived_from<IOOperation> auto& operation)
        {
            // ok?
            if(0 == epoll_ctl(this->epollFD, EPOLL_CTL_ADD, operation.fd, &operation.eventData))
                return;

            if(errno == EEXIST)
                // re-init event
                if(0 == epoll_ctl(this->epollFD, EPOLL_CTL_MOD, operation.fd, &operation.eventData))
                    return;

            throw std::system_error{errno, std::system_category(), strerror(errno)};
        }

        /*
         * Removed scheduled operation from pool.
         * Already scheduled operation will be immediately resumed and will throw IOOperationCancel exception.
         * If operation was not scheduled yet, nothing will happen.
         *
         * @operation Specifies IO operation object.
         */
        void removeScheduledOperation(std::derived_from<IOOperation> auto& operation)
        {
            if(0 != epoll_ctl(this->epollFD, EPOLL_CTL_DEL, operation.fd, {}))
                throw std::system_error{errno, std::system_category(), "epoll_ctl EPOLL_CTL_DEL"};

            // check, if operation was scheduled
            if(auto ptr = operation.eventData.data.ptr; ptr != nullptr)
            {
                // if yes, resume it and prepare throw
                operation.eventData.data.ptr = nullptr;
                IOOperation::CoroHandle::from_address(ptr).resume();
            }
        }

    private:
        tinycoro::Generator<IOOperation::CoroHandle> yieldAwaitingEvents(int timeout);

        std::unique_ptr<epoll_event[]> eventsList;
        const uint32_t eventPoolCount;
        int epollFD = -1;
    };

};     // namespace tinycoro::io
#endif // TINYCORO_IOCONTEXT_HPP
