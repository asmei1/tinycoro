//
// Created by Asmei on 12/13/2020.
//

#ifndef TINYCORO_IOASYNCSTDIN_HPP
#define TINYCORO_IOASYNCSTDIN_HPP
#include "IOOperation.hpp"
#include "IOContext.hpp"
#include <iostream>
#include <unistd.h>
namespace tinycoro::io
{
    class AsyncStdinOperation;

    class AsyncStdin
    {
    public:
        AsyncStdin(IOContext& context) : ioContext(context)
        {}

        AsyncStdin(AsyncStdin& t) = delete;
        AsyncStdin& operator=(AsyncStdin&) = delete;

        AsyncStdin& operator=(AsyncStdin&& t) = delete;
        AsyncStdin(AsyncStdin&& t) = delete;

        ~AsyncStdin() = default;

        //Version with coro support will be implemented later
        //AsyncStdinOperation readData(void* buffer, size_t bufferSize);
        ssize_t readData(void* buffer, size_t bufferSize);

    private:
        IOContext& ioContext;
    };
    class AsyncStdinOperation : public IOOperation
    {
    public:
        AsyncStdinOperation(IOContext& context) : IOOperation(context)
        {
            this->fd = STDIN_FILENO;
            this->settings.events = EPOLLIN | EPOLLONESHOT;
        }

        AsyncStdinOperation(AsyncStdinOperation& t) = delete;
        AsyncStdinOperation& operator=(AsyncStdinOperation&) = delete;

        AsyncStdinOperation& operator=(AsyncStdinOperation&& t) = delete;
        AsyncStdinOperation(AsyncStdinOperation&& t) = delete;

        bool await_ready() noexcept
        {
            return false;
        }
        void await_suspend(std::coroutine_handle<> awaitingCoro)
        {
            this->settings.data.ptr = awaitingCoro.address();

            this->ioContext.scheduleOperation(*this);
        }
        void await_resume() noexcept
        {}

        ssize_t readFromStream(void* buffer, size_t bufferSize){
            return ::read(this->fd, buffer, bufferSize);
        }
    };

//    AsyncStdinOperation AsyncStdin::readData(void* buffer, size_t bufferSize)
//    {
//        return AsyncStdinOperation(this->ioContext);
//    }
    ssize_t AsyncStdin::readData(void* buffer, size_t bufferSize)
    {
        return AsyncStdinOperation(this->ioContext).readFromStream(buffer, bufferSize);
    }
} // namespace tinycoro::io
#endif // TINYCORO_IOASYNCSTDIN_HPP
