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
     * TODO prepare description
     */
    class FireAndForget : std::suspend_never
    {
    public:
        struct FireAndForgetPromise;

        FireAndForget() = default;

        FireAndForget(FireAndForget&) = delete;
        FireAndForget& operator=(FireAndForget&) = delete;
        FireAndForget(FireAndForget&&) = delete;
        FireAndForget& operator=(FireAndForget&&) = delete;

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
        {
        }

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

    template <typename T> FireAndForget fireAndForget(T t)
    {
        if constexpr(std::is_invocable_v<T>)
        {
            co_await t();
        }
        else
        {
            co_await t;
        }
    }
} // namespace tinycoro

#endif // TINYCORO_FIREANDFORGET_HPP
