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
            ThreadPoolOperation(StaticCoroThreadPool* threadPool) : pool(threadPool)
            {}

            bool await_ready() noexcept;
            void await_suspend(std::coroutine_handle<> awaitingCoro);
            void await_resume() noexcept;

        private:
            friend class StaticCoroThreadPool;
            std::coroutine_handle<> awaitingCoro;
            StaticCoroThreadPool* pool;
        };

        ThreadPoolOperation resumeOperationOnPool()
        {
            return {this};
        }

        void waitForAllWorkers();
    private:
        void startWorker(std::stop_token stopToken);

        void planOperationWhenIsSuspended(ThreadPoolOperation* operation);

        std::condition_variable_any operationsCV;
        std::condition_variable waitForAllWorkersCV;
        std::mutex operationsQueueMutex;
        std::queue<ThreadPoolOperation*> operationsQueue;
        std::vector<std::jthread> threads;
    };

} // namespace tinycoro

#endif // TINYCORO_STATICCOROTHREADPOOL_HPP
