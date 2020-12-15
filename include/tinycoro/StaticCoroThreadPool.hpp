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
    class StaticCoroThreadPool
    {
    public:
        StaticCoroThreadPool(size_t threadCount = std::thread::hardware_concurrency());
        ~StaticCoroThreadPool();

        StaticCoroThreadPool(StaticCoroThreadPool&) = delete;
        StaticCoroThreadPool& operator=(StaticCoroThreadPool&) = delete;
        StaticCoroThreadPool(StaticCoroThreadPool&&) = delete;
        StaticCoroThreadPool& operator=(StaticCoroThreadPool&&) = delete;

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

        Scheduler getScheduler();

        [[nodiscard]] ThreadPoolOperation schedule()
        {
            return {*this};
        }

        size_t threadCount() const;
        void wait();

    private:
        void startWorker(std::stop_token stopToken);

        void scheduleOpOnPoolWhenIsSuspended(ThreadPoolOperation* operation);

        std::vector<std::jthread> workers;
        std::mutex operationsQueueMutex;
        std::condition_variable waitForAllWorkersCV;
        std::condition_variable_any operationsCV;
        std::queue<ThreadPoolOperation*> operationsQueue;
    };

    struct StaticCoroThreadPool::Scheduler
    {
        Scheduler(StaticCoroThreadPool& pool) : pool(pool)
        {}

        [[nodiscard]] StaticCoroThreadPool::ThreadPoolOperation schedule()
        {
            return {this->pool};
        }

    private:
        StaticCoroThreadPool& pool;
    };

} // namespace tinycoro

#endif // TINYCORO_STATICCOROTHREADPOOL_HPP
