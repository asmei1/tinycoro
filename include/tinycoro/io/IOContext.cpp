//
// Created by Asmei on 11/27/2020.
//
#include "IOContext.hpp"
#include <cstring>
#include <iostream>
#include <unistd.h>
using namespace tinycoro::io;

IOContext::IOContext(uint32_t eventPoolCount)
    : eventsList(std::make_unique<epoll_event[]>(eventPoolCount)),
      eventPoolCount(eventPoolCount)
{
    this->epollFD = epoll_create1(0);

    if(-1 == this->epollFD)
    {
        throw IOContextException{strerror(errno)};
    }
}

IOContext::~IOContext()
{
    close(this->epollFD);
}

#include <iostream>
tinycoro::Generator<epoll_event> IOContext::processAwaitingEvents(int timeout)
{
    std::cout << "Waiting for input...." << std::endl;

    // std::cout << fd << " " << events << " " << maxEvents << " " << timeout << std::endl;
    int eventCount = epoll_wait(this->epollFD, this->eventsList.get(), this->eventPoolCount, timeout);
    std::cout << eventCount << " ready events" << std::endl;

    if(eventCount == -1)
    {
        throw IOContextException(strerror(errno));
    }
    for(int i = 0; i < eventCount; ++i)
    {
        co_yield this->eventsList[i];
    }
}

IOContext::IOContextOperation IOContext::schedule()
{
    return {*this};
}
