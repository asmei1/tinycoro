//
// Created by Asmei on 11/4/2020.
//

#ifndef TINYCORO_CHRONOLITERALSCOAWAIT_HPP
#define TINYCORO_CHRONOLITERALSCOAWAIT_HPP
#include <chrono>
#include <coroutine>
#include <iostream>
#include <thread>

using namespace std::chrono_literals;

template <class Rep, class Period>
auto operator co_await(std::chrono::duration<Rep, Period> dur)
{
    struct awaiter
    {
        using clock = std::chrono::high_resolution_clock;
        clock::time_point resume_time;

        explicit awaiter(clock::duration dur) : resume_time(clock::now() + dur)
        {}

        bool await_ready()
        {
            return resume_time <= clock::now();
        }

        void await_suspend(std::coroutine_handle<> h)
        {
            std::thread([=]() {
                std::this_thread::sleep_until(resume_time);
                h.resume();
            }).detach();
        }

        void await_resume()
        {}
    };
    return awaiter{dur};
}

#endif // TINYCORO_CHRONOLITERALSCOAWAIT_HPP
