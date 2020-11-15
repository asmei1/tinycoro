//
// Created by Asmei on 11/15/2020.
//

#ifndef TINYCORO_TASK_H
#define TINYCORO_TASK_H
#include <coroutine>
#include <stdexcept>
#include <utility>
namespace tinycoro
{
    class Task
    {
    public:
        Task() : coroHandle(nullptr)
        {}
        Task(Task& t) = delete;
        Task(Task&& t) noexcept : coroHandle(std::exchange(t.coroHandle, {}))
        {}

        ~Task()
        {
            if(this->coroHandle)
            {
                this->coroHandle.destroy();
            }
        }

        class TaskPromise;
        using promise_type = TaskPromise;

        class TaskPromise
        {
        public:
            void return_void() const noexcept
            {}
            std::suspend_always initial_suspend() const noexcept
            {
                return {};
            }
            auto final_suspend() const noexcept
            {
                struct FinalAwaiter
                {
                    bool await_ready() const noexcept
                    {
                        return false;
                    }

                    std::coroutine_handle<> await_suspend(std::coroutine_handle<TaskPromise> coroHandle)
                    {
                        return coroHandle.promise().continuation;
                    }

                    void await_resume() const noexcept
                    {}
                };

                // temporary
                return FinalAwaiter{};
            }
            void unhandled_exception()
            {
                this->exceptionPtr = std::current_exception();
            }

            Task get_return_object() noexcept
            {
                return Task{std::coroutine_handle<TaskPromise>::from_promise(*this)};
            }

            std::exception_ptr exceptionPtr;
            std::coroutine_handle<> continuation;
        };

        auto operator co_await()
        {
            struct awaiter
            {
                awaiter(std::coroutine_handle<TaskPromise> coro) : coro(coro)
                {}
                bool await_ready() const noexcept
                {
                    return false;
                }

                std::coroutine_handle<> await_suspend(std::coroutine_handle<> continuation)
                {
                    this->coro.promise().continuation = continuation;

                    return this->coro;
                }

                void await_resume() const noexcept
                {}

            private:
                std::coroutine_handle<TaskPromise> coro;
            };

            return awaiter{this->coroHandle};
        }


    private:
        explicit Task(std::coroutine_handle<TaskPromise> coro) : coroHandle(coro)
        {}

        std::coroutine_handle<TaskPromise> coroHandle;
    };

} // namespace tinycoro

#endif // TINYCORO_TASK_H
