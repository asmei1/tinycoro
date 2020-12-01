//
// Created by Asmei on 10/27/2020.
//

#include "tinycoro/TinyCoro.hpp"
#include <cstring>
#include <iostream>
#include <ranges>
#include <sys/epoll.h>
#include <unistd.h>
#include <cstdlib>

int main()
{
    constexpr int MAX_EVENTS = 4;
    constexpr int READ_SIZE = 10;

    bool running = true;
    int eventCount = 0;
    size_t bytesRead = 0;
    char readBuffer[READ_SIZE + 1];
    std::array<epoll_event, MAX_EVENTS> events;
    epoll_event event;

    constexpr int stdinFD = 0;

    int epollFD = epoll_create1(0);

    if(-1 == epollFD)
    {
        std::cerr << "Error" << std::endl;
        exit(1);
    }

    event.events = EPOLLIN;
    event.data.fd = 0;
    if(-1 == epoll_ctl(epollFD, EPOLL_CTL_ADD, stdinFD, events.data()))
    {
        std::cerr << "Error" << std::endl;
        close(epollFD);
        exit(1);
    }

    while(running)
    {
        std::cout << "Waiting for input...." << std::endl;
        eventCount = epoll_wait(epollFD, events.data(), MAX_EVENTS, 10000);
        std::cout << eventCount << " ready events" << std::endl;
        if(eventCount == -1){
            std::cout << "ERROR! : " << strerror(errno) << std::endl;
        }
        for(int i = 0; i < eventCount; ++i)
        {
            std::cout << "Reading file description " << events[i].data.fd << std::endl;
            bytesRead = read(events[i].data.fd, readBuffer, READ_SIZE);
            readBuffer[bytesRead] = '\0';
            std::cout << "DATA: " << readBuffer << std::endl;

            if(!strncmp(readBuffer, "stop", 4)){
                running = false;
            }
        }
    }

    if(close(epollFD))
    {
        std::cerr << "Cannot close fd" << std::endl;
    }

    return 0;
}
