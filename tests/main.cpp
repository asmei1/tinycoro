//
// Created by Asmei on 10/27/2020.
//

#include "tinycoro/TinyCoro.hpp"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <unistd.h>

//tinycoro::FireAndForget readFromStdin(tinycoro::io::IOContext& context, bool& stopToken)
//{
//    constexpr int READ_SIZE = 10;
//    int eventCount = 0;
//    size_t bytesRead = 0;
//    char readBuffer[READ_SIZE + 1];
//
//    tinycoro::io::IOReadOnlyEvent e{context, STDIN_FILENO};
//    while(true)
//    {
//        try
//        {
//            std::cout << "Waiting... " << std::endl;
//            co_await e;
//            std::cout << "Reading file description " << e.getFD() << std::endl;
//            bytesRead = e.read(readBuffer, READ_SIZE);
//            readBuffer[bytesRead] = '\0';
//            std::cout << "DATA: " << readBuffer << std::endl;
//
//            if(!strncmp(readBuffer, "stop", 4))
//            {
//                stopToken = false;
//                break;
//            }
//
//            if(!strncmp(readBuffer, "remove", 6))
//            {
//                context.removeScheduledOperation(e);
//                break;
//            }
//        }
//        catch(std::system_error& e)
//        {
//            std::cerr << e.what() << std::endl;
//        }
//        catch(...)
//        {
//            std::cerr << "There was an error in coroutine" << std::endl;
//        }
//    }
//}

int main()
{
    constexpr int MAX_EVENTS = 4;
    tinycoro::io::IOContext ioContext{MAX_EVENTS};

    bool running = true;

//    readFromStdin(ioContext, running);
    while(running)
    {
        ioContext.processAwaitingEvents(10000);
    }

    return 0;
}
