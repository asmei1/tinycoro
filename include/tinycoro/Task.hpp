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
    /*
     * Coroutine which will be not executed until it won't be called with co_await.
     *
     * Coroutine body which will returns a tiny::Task<T> have to contains the co_await or co_return keyword.
     * co_yield is forbidden here.
     *
     * Until coroutine will be not executed with co_await, it will be suspend in awaiting state.
     * It will be resumed later on thread where it will be co_await'ed.
     * Any exception will stored and rethrow in resume thread.
     *
     * If tiny::Task<T> object was destroyed before coroutine function was executed, all memory is frees automatically.
     */
    template <typename T = void>
    class Task
    {
        class TaskPromiseBase;
        class TaskPromise;

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

        using promise_type = TaskPromise;
        using promise_coro_handle = std::coroutine_handle<promise_type>;

        auto operator co_await() const &
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

        auto operator co_await() const &&
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
                    return std::move(this->coro.promise().result());
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

        class TaskPromiseBase
        {
        protected:
            TaskPromiseBase() = default;
            virtual ~TaskPromiseBase() = default;


            friend class Task;
            std::coroutine_handle<> continuation;
            std::exception_ptr exceptionPtr;

        public:
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

                return FinalAwaiter{};
            }
            void unhandled_exception()
            {
                this->exceptionPtr = std::current_exception();
            }

            void rethrowExceptionIfExists()
            {
                if(this->exceptionPtr)
                {
                    std::rethrow_exception(this->exceptionPtr);
                }
            }
        };

        class TaskPromise : public TaskPromiseBase
        {
            T value;

        public:
            void return_value(T value)
            {
                this->value = value;
            }

            T&& result()
            {
                if(this->exceptionPtr)
                {
                    std::rethrow_exception(this->exceptionPtr);
                }

                return std::move(this->value);
            }

            Task get_return_object() noexcept
            {
                return Task{promise_coro_handle::from_promise(*this)};
            }
        };
    };

    template <>
    class Task<void>::TaskPromise : public TaskPromiseBase
    {
    public:
        void return_void()
        {}

        void result()
        {
            if(exceptionPtr)
            {
                std::rethrow_exception(this->exceptionPtr);
            }
        }

        Task get_return_object() noexcept
        {
            return Task{promise_coro_handle::from_promise(*this)};
        }
    };

} // namespace tinycoro

#endif // TINYCORO_TASK_H
