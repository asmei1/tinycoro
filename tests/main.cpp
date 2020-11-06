//
// Created by Asmei on 10/27/2020.
//

#include "tinycoro/ChronoLiteralsCoAwait.hpp"
#include "tinycoro/FireAndForget.hpp"
#include "tinycoro/StaticCoroThreadPool.hpp"
#include <chrono>
#include <coroutine>
#include <iostream>
#include <thread>

tinycoro::FireAndForget test2()
{
    std::cout << "Hello" << std::endl;
}

tinycoro::FireAndForget sampleTestOfThreadPool(tinycoro::StaticCoroThreadPool& pool)
{
    std::cout << "Thread ID before co_await: " << std::this_thread::get_id() << std::endl;
    co_await pool.resumeOperationOnPool();
    std::this_thread::sleep_for(10s);
    std::cout << "Thread ID after co_await: " << std::this_thread::get_id() << std::endl;
}
tinycoro::FireAndForget sampleTestOfThreadPoolWithDelay(tinycoro::StaticCoroThreadPool& pool)
{
    std::cout << "Thread ID before co_await: " << std::this_thread::get_id() << std::endl;
    co_await pool.resumeOperationOnPool();
    std::this_thread::sleep_for(3s);
    std::cout << "Thread ID after co_await: " << std::this_thread::get_id() << std::endl;
}

int main()
{
    tinycoro::StaticCoroThreadPool pool;
    sampleTestOfThreadPool(pool);
    sampleTestOfThreadPoolWithDelay(pool);
    pool.waitForAllWorkers();
    return 0;
}
