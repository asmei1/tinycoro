//
// Created by Asmei on 10/27/2020.
//

#include "tinycoro/TinyCoro.hpp"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <unistd.h>

tinycoro::Task<uint64_t> startComputingOnThread(tinycoro::scheduler_trait auto scheduler,
                                                tinycoro::io::EpollAsyncAutoResetEvent& e)
{
    co_await e;
    std::cout << "Prepare to computation, moving to new thread (old thread id = " << std::this_thread::get_id()
              << ")\n";
    co_await scheduler.schedule();

    std::cout << "Moved to new thread (thread id = " << std::this_thread::get_id() << "), starting computations...\n";
    // some computations
    uint64_t result = 0;
    for(auto i : tinycoro::range(0, 1000000))
    {
        result += i;
    }

    co_return result;
}

// This function will wait in separate thread for computation result
tinycoro::FireAndForget waitingForComputationResult(tinycoro::scheduler_trait auto scheduler,
                                                    tinycoro::io::EpollAsyncAutoResetEvent& e)
{
    co_await scheduler.schedule();

    std::cout << "Waiting for computation result...(on thread = " << std::this_thread::get_id() << ")\n";
    uint64_t computationResult = co_await startComputingOnThread(scheduler, e);

    std::cout << "We got it! Result = " << computationResult << std::endl;
}

tinycoro::FireAndForget readFromStdin(tinycoro::io::IOContext& context, bool& stopToken,
                                      tinycoro::io::EpollAsyncAutoResetEvent& eTask)
{
    constexpr int READ_SIZE = 10;
    int eventCount = 0;
    size_t bytesRead = 0;
    char readBuffer[READ_SIZE + 1];

    tinycoro::io::AsyncStdinOperation e{context};
    while(true)
    {
        try
        {
            co_await e;
            std::cout << "Reading file description " << e.getFD() << ", on thread id = " << std::this_thread::get_id()
                      << std::endl;
            bytesRead = e.readFromStream(readBuffer, READ_SIZE);
            readBuffer[bytesRead] = '\0';
            std::cout << "DATA: " << readBuffer << std::endl;

            if(!strncmp(readBuffer, "stop", 4))
            {
                stopToken = false;
                break;
            }

            if(!strncmp(readBuffer, "start", 5))
            {
                eTask.set();
            }
        }
        catch(std::system_error& e)
        {
            std::cerr << e.what() << std::endl;
        }
        catch(...)
        {
            std::cerr << "There was an error in coroutine" << std::endl;
        }
    }
}

tinycoro::FireAndForget processEpollEvents(tinycoro::scheduler_trait auto scheduler, tinycoro::io::IOContext& ioContext,
                                           bool onThreadPool = false)
{
    if(onThreadPool)
        co_await scheduler.schedule();

    std::cout << "Started, thread id = " << std::this_thread::get_id() << std::endl;

    tinycoro::io::EpollAsyncAutoResetEvent eTask{ioContext};
    waitingForComputationResult(scheduler, eTask);

    bool running = true;
    readFromStdin(ioContext, running, eTask);
    while(running)
    {
        ioContext.processAwaitingEvents(-1);
    }

    std::cout << "Closed, thread id = " << std::this_thread::get_id() << std::endl;
}

int main()
{
    std::cout << "Main thread id = " << std::this_thread::get_id() << "\n";

    constexpr int MAX_EVENTS = 4;
    tinycoro::io::IOContext ioContext{MAX_EVENTS};
    tinycoro::StaticCoroThreadPool threadPool{4};

    processEpollEvents(threadPool.getScheduler(), ioContext, true);

    threadPool.wait();
    return 0;
}
