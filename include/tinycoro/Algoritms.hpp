//
// Created by Asmei on 11/25/2020.
//

#ifndef TINYCORO_ALGORITMS_HPP
#define TINYCORO_ALGORITMS_HPP
#include "Generator.hpp"
namespace tinycoro
{
    template <std::incrementable T>
    tinycoro::Generator<T> range(T begin)
    {
        while(true)
        {
            co_yield begin++;
        }
    }

    template <std::incrementable T>
    tinycoro::Generator<T> range(T begin, T end)
    {
        while(begin != end)
        {
            co_yield begin++;
        }
    }

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

}
#endif // TINYCORO_ALGORITMS_HPP
