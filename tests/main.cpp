//
// Created by Asmei on 10/27/2020.
//

#include "tinycoro/TinyCoro.hpp"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <unistd.h>

tinycoro::Task<> readFromStdin(tinycoro::io::IOContext& context, bool& stopToken)
{
    constexpr int READ_SIZE = 10;
    int eventCount = 0;
    size_t bytesRead = 0;
    char readBuffer[READ_SIZE + 1];

    tinycoro::io::IOEvent e{context};
    while(true)
    {
        try
        {
            co_await e;
            std::cout << "Reading file description " << e.e.data.fd << std::endl;
            bytesRead = read(e.e.data.fd, readBuffer, READ_SIZE);
            readBuffer[bytesRead] = '\0';
            std::cout << "DATA: " << readBuffer << std::endl;

            if(!strncmp(readBuffer, "stop", 4))
            {
                stopToken = false;
            }
        }
        catch(std::runtime_error& e)
        {
            std::cerr << e.what() << std::endl;
        }
        catch(...){

        }
    }
}

int main()
{
    constexpr int MAX_EVENTS = 4;
    tinycoro::io::IOContext ioContext{MAX_EVENTS};

    bool running = true;

    tinycoro::fireAndForget(readFromStdin(ioContext, running));
    while(running)
    {
        ioContext.processAwaitingEvents(10000);
    }
    return 0;
}
