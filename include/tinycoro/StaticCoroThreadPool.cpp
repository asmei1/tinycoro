//
// Created by Asmei on 11/6/2020.
//
#include "StaticCoroThreadPool.hpp"
#include <functional>
#include <iostream>

using namespace tinycoro;

StaticCoroThreadPool::StaticCoroThreadPool(size_t threadCount)
{
    for(int i = 0; i < threadCount; ++i)
    {
        this->workers.emplace_back([&](std::stop_token stopToken) { startWorker(stopToken); });
    }
}

StaticCoroThreadPool::~StaticCoroThreadPool()
{
    for(auto& t : this->workers)
    {
        t.request_stop();
    }

    for(auto& t : this->workers)
    {
        t.join();
    }
}

void StaticCoroThreadPool::startWorker(std::stop_token stopToken)
{
    while(!stopToken.stop_requested())
    {
        std::unique_lock lock(this->operationsQueueMutex);
        this->operationsCV.wait(lock, stopToken, [&]() { return not this->operationsQueue.empty(); });

        // If thread was stopped, stop worker too
        if(stopToken.stop_requested())
        {
            return;
        }

        // resume coroutine
        auto coro = this->operationsQueue.front();
        this->operationsQueue.pop();
        this->waitForAllWorkersCV.notify_one();
        coro->awaitingCoro.resume();
    }
}

void StaticCoroThreadPool::planOperationWhenIsSuspended(StaticCoroThreadPool::ThreadPoolOperation* operation)
{
    std::unique_lock lock(this->operationsQueueMutex);

    this->operationsQueue.push(operation);
    this->operationsCV.notify_one();
}

void StaticCoroThreadPool::waitForAllWorkers()
{
    std::unique_lock lock(this->operationsQueueMutex);
    this->waitForAllWorkersCV.wait(lock, [&]() { return this->operationsQueue.empty(); });
}

bool StaticCoroThreadPool::ThreadPoolOperation::await_ready() noexcept
{
    return false;
}

void StaticCoroThreadPool::ThreadPoolOperation::await_suspend(std::coroutine_handle<> awaitingCoro)
{
    // Store given coro handle and put operation into
    this->awaitingCoro = awaitingCoro;
    this->pool->planOperationWhenIsSuspended(this);
}

void StaticCoroThreadPool::ThreadPoolOperation::await_resume() noexcept
{}
