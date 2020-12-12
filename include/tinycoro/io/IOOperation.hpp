//
// Created by Asmei on 12/12/2020.
//

#ifndef TINYCORO_IOOPERATION_HPP
#define TINYCORO_IOOPERATION_HPP

namespace tinycoro::io
{
    class IOContext;

    class IOOperation
    {
    public:
        virtual ~IOOperation() = default;

        using CoroHandle = std::coroutine_handle<>;

        int getFD() const
        {
            return this->fd;
        }

    protected:
        IOOperation(IOContext& context) : ioContext(context)
        {}

        friend class IOContext;

        uint64_t fd;
        IOContext& ioContext;
        epoll_event settings;
    };
} // namespace tinycoro::io
#endif // TINYCORO_IOOPERATION_HPP
