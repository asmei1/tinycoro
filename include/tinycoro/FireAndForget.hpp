//
// Created by Asmei on 10/27/2020.
//

#ifndef TINYCORO_FIREANDFORGET_HPP
#define TINYCORO_FIREANDFORGET_HPP

#include <coroutine>
#include <exception>
#include <type_traits>

namespace tinycoro
{
    /*
     * A simple helper class for using coroutine.
     * Has the co_await operator deleted, so the returned result cannot be co_awaited.
     * Fire and forget, you are not interested in its result or whether it has managed to do something
     * Any exception will terminate application.
     */
    class FireAndForget
    {
    public:
        struct FireAndForgetPromise;

        FireAndForget() = default;

        FireAndForget(FireAndForget&) = delete;
        FireAndForget& operator=(FireAndForget&) = delete;
        FireAndForget(FireAndForget&&) = delete;
        FireAndForget& operator=(FireAndForget&&) = delete;

        // Deleted co_await operator
        auto operator co_await() = delete;

        using promise_type = FireAndForgetPromise;
    };

    struct FireAndForget::FireAndForgetPromise
    {
        FireAndForget get_return_object() const noexcept
        {
            return {};
        }

        void return_void() const noexcept
        {}

        std::suspend_never initial_suspend() const noexcept
        {
            return {};
        }

        std::suspend_never final_suspend() const noexcept
        {
            return {};
        }

        void unhandled_exception() const noexcept
        {
            std::terminate();
        }
    };


} // namespace tinycoro

#endif // TINYCORO_FIREANDFORGET_HPP
