//
// Created by Asmei on 11/25/2020.
//

#ifndef TINYCORO_ALGORITHMS_HPP
#define TINYCORO_ALGORITHMS_HPP
#include "FireAndForget.hpp"
#include "Generator.hpp"
namespace tinycoro
{
    /*
     * Create endless stream, yielding objects from begin (inclusive).
     */
    template <std::incrementable T>
    tinycoro::Generator<T> range(T begin)
    {
        while(true)
        {
            co_yield begin++;
        }
    }

    /*
     * Yield objects from begin (inclusive) to end (enxclusive) with incremental.
     */
    template <std::incrementable T>
    tinycoro::Generator<T> range(T begin, T end)
    {
        while(begin != end)
        {
            co_yield begin++;
        }
    }

    /*
     * Yield objects from begin (inclusive) to end (enxclusive) with given step.
     */
    template <typename T, typename Step>
    requires requires (T x, Step s) { x + s; }
    tinycoro::Generator<T> range(T begin, T end, Step step)
    {
        while(begin < end)
        {
            co_yield begin;
            begin += step;
        }
    }


    /*
     * Helper function to execute any coroutine as fire and forget function.
     * It could receive any awaitable object or coroutine function.
     */
    template <typename T>
    FireAndForget fireAndForget(T t)
    {
        if constexpr(std::is_invocable_v<T>)
        {
            co_await t();
        }
        else
        {
            co_await std::move(t);
        }
    }
}
#endif // TINYCORO_ALGORITHMS_HPP
