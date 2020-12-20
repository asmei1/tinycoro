//
// Created by Asmei on 12/13/2020.
//

#ifndef TINYCORO_IOASYNCSTDIN_HPP
#define TINYCORO_IOASYNCSTDIN_HPP
#include "IOContext.hpp"
#include "IOOperation.hpp"
namespace tinycoro::io
{
    /*
     * AsyncStdin supports reading standard input stream with coroutine.
     * Required IOContext to be resumed when reading is available.
     */
    class AsyncStdin
    {
        struct StdinOperation;

    public:
        /*
         * Initialize AsyncStdin object.
         */
        AsyncStdin(IOContext& context) : ioContext(context)
        {}

        //Non-copyable
        AsyncStdin(AsyncStdin& t) = delete;
        AsyncStdin& operator=(AsyncStdin&) = delete;


        ~AsyncStdin() = default;

        /*
         * Reads data from stdin.
         * Returns awaitable object.
         *
         * @param buffer - pointer to allocated buffer, where data will be stored.
         * @param bufferSize - the maximum size of the buffer where stdin data will be written.
         */
        StdinOperation readData(void* buffer, size_t bufferSize);

    private:
        IOContext& ioContext;
    };

    struct AsyncStdin::StdinOperation : public IOOperation
    {
    public:
        bool await_ready() noexcept
        {
            return false;
        }

        void await_suspend(std::coroutine_handle<> awaitingCoro)
        {
            this->eventData.data.ptr = awaitingCoro.address();

            this->ioContext.scheduleOperation(*this);
        }

        /*
         * Returns number of bytes which was read.
         *
         * @throw IOOperationCancel exception if operation was canceled.
         */
        auto await_resume() noexcept
        {
            this->throwExceptionIfCanceled();

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
            this->eventData.events = EPOLLIN | EPOLLONESHOT;
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
