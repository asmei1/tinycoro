//
// Created by Asmei on 10/27/2020.
//

#include "tinycoro/TinyCoro.hpp"
#include <chrono>
#include <coroutine>
#include <iostream>
#include <thread>
using namespace std::chrono_literals;

tinycoro::FireAndForget test2()
{
    std::cout << "Hello" << std::endl;
}

tinycoro::FireAndForget sampleTestOfThreadPool(tinycoro::StaticCoroThreadPool& pool)
{
    std::cout << "Thread ID before co_await: " << std::this_thread::get_id() << std::endl;
    co_await pool.getScheduler().schedule();
    std::cout << "Before sleep" << std::endl;
    std::this_thread::sleep_for(3s);
    std::cout << "After sleep, thread ID: " << std::this_thread::get_id() << std::endl;
}

tinycoro::FireAndForget sampleTestOfThreadPoolWithDelay(tinycoro::StaticCoroThreadPool& pool)
{
    std::cout << "Thread ID before co_await with delay: " << std::this_thread::get_id() << std::endl;
    co_await pool.getScheduler().schedule();
    std::this_thread::sleep_for(3s);
    std::cout << "Thread ID after co_await with delay: " << std::this_thread::get_id() << std::endl;
}

tinycoro::FireAndForget sampleWithTraitArgument(tinycoro::scheduler_trait auto scheduler)
{
    std::cout << "Thread ID before co_await: " << std::this_thread::get_id() << std::endl;
    co_await scheduler.schedule();
    std::cout << "Before sleep" << std::endl;
    std::this_thread::sleep_for(3s);
    std::cout << "After sleep, thread ID: " << std::this_thread::get_id() << std::endl;
}

int main()
{
    tinycoro::StaticCoroThreadPool pool{4};
    std::cout << "Thread count " << pool.threadCount() << std::endl;
    for(int i = 0; i < 4; i++)
    {
        std::cout << i << std::endl;
        sampleWithTraitArgument(pool.getScheduler());
    }
    pool.wait();
    return 0;
}
