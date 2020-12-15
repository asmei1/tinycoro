//
// Created by Asmei on 12/13/2020.
//

#ifndef TINYCORO_IOASYNCSTDIN_HPP
#define TINYCORO_IOASYNCSTDIN_HPP
#include "IOContext.hpp"
#include "IOOperation.hpp"
#include <iostream>
#include <unistd.h>
namespace tinycoro::io
{
    class AsyncStdin
    {
        struct StdinOperation;

    public:
        AsyncStdin(IOContext& context) : ioContext(context)
        {}

        AsyncStdin(AsyncStdin& t) = delete;
        AsyncStdin& operator=(AsyncStdin&) = delete;

        AsyncStdin& operator=(AsyncStdin&& t) = delete;
        AsyncStdin(AsyncStdin&& t) = delete;

        ~AsyncStdin() = default;

        StdinOperation readData(void* buffer, size_t bufferSize);

    private:
        IOContext& ioContext;
    };

    struct AsyncStdin::StdinOperation : public IOOperation
    {
    public:
        StdinOperation(StdinOperation& t) = delete;
        StdinOperation& operator=(StdinOperation&) = delete;

        StdinOperation& operator=(StdinOperation&& t) = delete;
        StdinOperation(StdinOperation&& t) = delete;

        bool await_ready() noexcept
        {
            return false;
        }

        void await_suspend(std::coroutine_handle<> awaitingCoro)
        {
            this->settings.data.ptr = awaitingCoro.address();

            this->ioContext.scheduleOperation(*this);
        }

        auto await_resume() noexcept
        {
            return ::read(this->fd, this->buffer, this->bufferSize);
        }

    private:
        friend class AsyncStdin;

        StdinOperation(IOContext& context, void* buffer, size_t bufferSize)
            : IOOperation(context),
              buffer(buffer),
              bufferSize(bufferSize)
        {
            this->fd = STDIN_FILENO;
            this->settings.events = EPOLLIN | EPOLLONESHOT;
        }

        void* buffer;
        size_t bufferSize;
    };

    AsyncStdin::StdinOperation AsyncStdin::readData(void* buffer, size_t bufferSize)
    {
        return StdinOperation(this->ioContext, buffer, bufferSize);
    }


} // namespace tinycoro::io
#endif // TINYCORO_IOASYNCSTDIN_HPP
