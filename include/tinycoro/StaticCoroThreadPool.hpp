//
// Created by Asmei on 11/6/2020.
//

#ifndef TINYCORO_STATICCOROTHREADPOOL_HPP
#define TINYCORO_STATICCOROTHREADPOOL_HPP
#include <condition_variable>
#include <coroutine>
#include <queue>
#include <thread>
#include <vector>

namespace tinycoro
{
    /*
     *  StaticCoroThreadPool is a static thread pool class, which handle scheduled coroutines on threads.
     */
    class StaticCoroThreadPool
    {
    public:
        /*
         * Initialize a thread pool with given number of threads.
         * It creates worker threads and prepare them for coroutines.
         *
         * @param threadCount Number of thread in this thread pool. Default value is detected from system.
         */
        StaticCoroThreadPool(size_t threadCount = std::thread::hardware_concurrency());
        ~StaticCoroThreadPool();

        //Non-copyable
        StaticCoroThreadPool(StaticCoroThreadPool&) = delete;
        StaticCoroThreadPool& operator=(StaticCoroThreadPool&) = delete;


        /*
         * ThreadPoolOperation is a awaitable type to schedule coroutine on thread pool.
         */
        struct ThreadPoolOperation
        {
        public:
            ThreadPoolOperation(StaticCoroThreadPool& pool) : pool(pool)
            {}

            bool await_ready() noexcept;
            void await_suspend(std::coroutine_handle<> awaitingCoro);
            void await_resume() noexcept;

        private:
            friend class StaticCoroThreadPool;

            std::coroutine_handle<> awaitingCoro;
            StaticCoroThreadPool& pool;
        };

        struct Scheduler;

        /*
         * Returns lightweight Scheduler object.
         */
        [[nodiscard]] Scheduler getScheduler();

        /*
         * Schedule coroutine, where this function was executed, on thread pool.
         */
        [[nodiscard]] ThreadPoolOperation schedule()
        {
            return {*this};
        }

        /*
         * Returns a number of threads
         */
        size_t threadCount() const;

        /*
         * Wait until all threads finished their jobs.
         * If any thread hangs, this function will run indefinitely.
         * Function does not close worker threads, only wait until all of them are not working,
         * ie. there are nothing to do.
         * Blocks scheduling new coroutines until queued so far coroutines ends.
         */
        void wait();


    private:
        /*
         * Send stop request to all worker threads and joins them.
         * It waits until all worker threads finished their actual jobs.
         */
        void shutdown();

        void startWorker(std::stop_token stopToken);

        void scheduleOpOnPoolWhenIsSuspended(ThreadPoolOperation* operation);

        std::vector<std::jthread> workers;
        std::mutex operationsQueueMutex;
        std::queue<ThreadPoolOperation*> operationsQueue;

        std::condition_variable waitForAllWorkersCV;
        std::condition_variable_any operationsCV;
    };

    /*
     * Scheduler is lightweight object for scheduling coroutines on parent thread pool.
     */
    struct StaticCoroThreadPool::Scheduler
    {
        explicit Scheduler(StaticCoroThreadPool& pool) : pool(pool)
        {}

        /*
         * Schedule coroutine, where this function was executed, on parent thread pool.
         */
        [[nodiscard]] StaticCoroThreadPool::ThreadPoolOperation schedule()
        {
            return {this->pool};
        }

    private:
        StaticCoroThreadPool& pool;
    };

} // namespace tinycoro

#endif // TINYCORO_STATICCOROTHREADPOOL_HPP
