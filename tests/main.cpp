//
// Created by Asmei on 10/27/2020.
//

#include "tinycoro/TinyCoro.hpp"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <unistd.h>

tinycoro::FireAndForget asyncAutoEventTest(tinycoro::StaticCoroThreadPool& p, tinycoro::io::EpollAsyncAutoResetEvent& e)
{
    co_await p.scheduleOnPool();

    std::cout << "Before co " <<  "\n";
    co_await e;
    std::cout << "After co " << "\n";
    co_await e;
    std::cout << "Before next co\n";
    co_await e;
    std::cout << "CLOOOSING\n";
}

tinycoro::FireAndForget readFromStdin(tinycoro::io::IOContext& context, bool& stopToken, tinycoro::io::EpollAsyncAutoResetEvent& eTask)
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
            std::cout << "Reading file description " << 0 << std::endl;
            bytesRead = e.readFromStream(readBuffer, READ_SIZE);
            readBuffer[bytesRead] = '\0';
            std::cout << "DATA: " << readBuffer << std::endl;

            if(!strncmp(readBuffer, "stop", 4))
            {
                stopToken = false;
                break;
            }

            if(!strncmp(readBuffer, "remove", 6))
            {
                context.removeScheduledOperation(e);
                break;
            }

            if(!strncmp(readBuffer, "set", 3))
            {
                std::cout << "Set executed\n";
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

int main()
{
    constexpr int MAX_EVENTS = 4;
    tinycoro::io::IOContext ioContext{MAX_EVENTS};
    tinycoro::StaticCoroThreadPool threadPool;

    tinycoro::io::EpollAsyncAutoResetEvent eTask{ioContext};
    asyncAutoEventTest(threadPool, eTask);

    bool running = true;
    readFromStdin(ioContext, running, eTask);
    while(running)
    {
        std::cout << "Waiting for events... " << std::endl;
        ioContext.processAwaitingEvents(10000);
    }
    threadPool.wait();
    return 0;
}
