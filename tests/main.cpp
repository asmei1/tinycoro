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

tinycoro::Task<float> completes_synchronously1()
{
    std::cout << "I'm trying to do something!\n";
    co_return 12;
}
tinycoro::Task<int> completes_synchronously()
{
    auto i = co_await completes_synchronously1();
    std::cout << "I'm trying to do something!\n";
    co_return 12;
}

tinycoro::FireAndForget sampleWithTraitArgument(tinycoro::scheduler_trait auto scheduler)
{
    co_await completes_synchronously();
    std::cout << "Thread ID before co_await: " << std::this_thread::get_id() << std::endl;
    co_await scheduler.schedule();
    std::cout << "Before sleep" << std::endl;
    std::this_thread::sleep_for(3s);
    std::cout << "After sleep, thread ID: " << std::this_thread::get_id() << std::endl;
}

tinycoro::Task<void> loop_synchronously(long count)
{
    std::cout << "loop_synchronously(" << count << ")" << std::endl;
    for(long i = 0; i < count; ++i)
    {
        co_await completes_synchronously();
    }
    std::cout << "loop_synchronously(" << count << ") returning" << std::endl;
}

tinycoro::FireAndForget runner()
{
    int result = co_await completes_synchronously();
    std::cout << "Result " << result << std::endl;
}

tinycoro::Generator<int> one_two_three()
{
    co_yield 1;
    co_yield 2;
    co_yield 3;
}

tinycoro::Task<void> runGeneratorOnAnotherThread(tinycoro::Generator<int> generator,
                                                 tinycoro::scheduler_trait auto pool)
{
    std::cout << "ID:" << std::this_thread::get_id() << std::endl;
    co_await pool.schedule();
    for(auto i : generator)
    {
        std::cout << "From range function on other pool: " << i << std::endl;
    }
    std::cout << "PO ID:" << std::this_thread::get_id() << std::endl;
}

int main()
{
    std::vector<int> t = {1, 2, 3, 4, 6, 7, 8};
    for(auto i : tinycoro::range(t.begin(), t.end()))
    {
        std::cout << "From range function: " << *i << std::endl;
    }
    for(auto i : tinycoro::range(t.begin(), t.end(), 2))
    {
        std::cout << "From range function: " << *i << std::endl;
    }

    for(auto i : tinycoro::range(0., 10., 1.5))
    {
        std::cout << "From range function: " << i << std::endl;
    }

    //    runner();
    //    sampleWithTraitArgument(pool.getScheduler());

    //    runner();
    //    tinycoro::fireAndForget(loop_synchronously(1000));
    //    loop_synchronously(1'000'000'000);
    //    tinycoro::StaticCoroThreadPool pool{4};
    //    std::cout << "Thread count " << pool.threadCount() << std::endl;
    //    for(int i = 0; i < 4; i++)
    //    {
    //        std::cout << i << std::endl;
    //        sampleWithTraitArgument(pool.getScheduler());
    //    }
    //    pool.wait();

    return 0;
}
