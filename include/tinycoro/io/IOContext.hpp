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
 *
 * . Close fd
*/
namespace tinycoro::io
{
    class IOContext
    {
    public:
        IOContext();
        ~IOContext();

        IOContext(IOContext&) = delete;
        IOContext& operator=(IOContext&) = delete;
        IOContext(IOContext&&) = delete;
        IOContext& operator=(IOContext&&) = delete;

        struct Scheduler;
        struct IOContextOperation;

        IOContextOperation schedule();

        void processAwaitingEvents();

    private:

    };

    struct IOContext::IOContextOperation
    {
        IOContextOperation(IOContext& pool) : pool(pool)
        {}

        [[nodiscard]] IOContext::IOContextOperation schedule()
        {
            return {this->pool};
        }

    private:
        IOContext& pool;
    };

    struct IOContext::Scheduler
    {
        Scheduler(IOContext& pool) : pool(pool)
        {}

        [[nodiscard]] IOContext::IOContextOperation schedule()
        {
            return {this->pool};
        }

    private:
        IOContext& pool;
    };

};     // namespace tinycoro::io
#endif // TINYCORO_IOCONTEXT_HPP
