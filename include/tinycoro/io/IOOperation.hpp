//
// Created by Asmei on 12/12/2020.
//

#ifndef TINYCORO_IOOPERATION_HPP
#define TINYCORO_IOOPERATION_HPP
namespace tinycoro::io
{
    struct IOOperationCancel
    {
    };

    class IOContext;

    class IOOperation
    {
    public:
        virtual ~IOOperation() = default;

        using CoroHandle = std::coroutine_handle<>;

        /*
         * Returns file description of operation.
         */
        int getFD() const
        {
            return this->fd;
        }

    protected:
        IOOperation(IOContext& context) : ioContext(context)
        {}

        /*
         * Checks, if operation was canceled.
         *
         * This function must be invoke in  await_resume function in each derived from IOOperation class as first
         * statement!
         *
         * @throw IOOperationCancel exception if operation was canceled.
         */
        void throwExceptionIfCanceled()
        {
            if(not this->eventData.data.ptr)
            {
                throw IOOperationCancel{};
            }
        }
        friend class IOContext;

        uint64_t fd;
        IOContext& ioContext;
        epoll_event eventData = {};
    };
} // namespace tinycoro::io
#endif // TINYCORO_IOOPERATION_HPP
