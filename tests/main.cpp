//
// Created by Asmei on 10/27/2020.
//

#include "tinycoro/TinyCoro.hpp"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <unistd.h>

int main()
{
    constexpr int MAX_EVENTS = 4;
    constexpr int READ_SIZE = 10;
    tinycoro::io::IOContext ioContext{MAX_EVENTS};

    bool running = true;
    int eventCount = 0;
    size_t bytesRead = 0;
    char readBuffer[READ_SIZE + 1];
    epoll_event event;

    constexpr int stdinFD = 0;

    event.events = EPOLLIN;
    event.data.fd = 0;
    if(-1 == epoll_ctl(ioContext.epollFD, EPOLL_CTL_ADD, stdinFD, &event))
    {
        std::cerr << "Error" << strerror(errno) << std::endl;
        exit(1);
    }

    while(running)
    {
        try
        {
            for(auto& e : ioContext.processAwaitingEvents(10000))
            {
                std::cout << "Reading file description " << e.data.fd << std::endl;
                bytesRead = read(e.data.fd, readBuffer, READ_SIZE);
                readBuffer[bytesRead] = '\0';
                std::cout << "DATA: " << readBuffer << std::endl;

                if(!strncmp(readBuffer, "stop", 4))
                {
                    running = false;
                }
            }
        }
        catch(std::runtime_error& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }
    return 0;
}
