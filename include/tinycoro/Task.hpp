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
    template <typename T = void>
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

        template<typename U>
        class TaskPromise;

        using promise_type = TaskPromise<T>;
        using promise_coro_handle = std::coroutine_handle<promise_type>;

        template <typename U = T>
        class TaskPromise
        {
            std::exception_ptr exceptionPtr;
            U value;

        public:
            void return_value(U value)
            {
                this->value = value;
            }

            U& result()
            {
                if(exceptionPtr)
                {
                    std::rethrow_exception(this->exceptionPtr);
                }

                return this->value;
            }

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

                    std::coroutine_handle<> await_suspend(promise_coro_handle coroHandle)
                    {
                        return coroHandle.promise().continuation;
                    }

                    void await_resume()
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
                return Task{promise_coro_handle::from_promise(*this)};
            }

            void rethrowExceptionIfExists(){
                if(this->exceptionPtr){
                    std::rethrow_exception(this->exceptionPtr);
                }
            }

            std::coroutine_handle<> continuation;
        };

        auto operator co_await()
        {
            struct awaiter
            {
                awaiter(promise_coro_handle coro) : coro(coro)
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

                auto await_resume()
                {
                    return this->coro.promise().result();
                }

            private:
                promise_coro_handle coro;
            };

            return awaiter{this->coroHandle};
        }

    private:
        explicit Task(promise_coro_handle coro) : coroHandle(coro)
        {}

        promise_coro_handle coroHandle;
    };

    template<> template <>
    class Task<void>::TaskPromise<void>
    {
        std::exception_ptr exceptionPtr;

    public:
        void return_void()
        {
        }

        void result()
        {
            if(exceptionPtr)
            {
                std::rethrow_exception(this->exceptionPtr);
            }
        }

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

                std::coroutine_handle<> await_suspend(promise_coro_handle coroHandle)
                {
                    return coroHandle.promise().continuation;
                }

                void await_resume()
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
            return Task{promise_coro_handle::from_promise(*this)};
        }

        void rethrowExceptionIfExists(){
            if(this->exceptionPtr){
                std::rethrow_exception(this->exceptionPtr);
            }
        }

        std::coroutine_handle<> continuation;
    };

} // namespace tinycoro

#endif // TINYCORO_TASK_H
