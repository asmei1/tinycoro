//
// Created by Asmei on 11/15/2020.
//

#ifndef TINYCORO_TINYCORO_HPP
#define TINYCORO_TINYCORO_HPP

#include "Algorithms.hpp"
#include "FireAndForget.hpp"
#include "Generator.hpp"
#include "StaticCoroThreadPool.hpp"
#include "Task.hpp"
#include "Traits.hpp"

//IO available only on Linux
#ifdef __linux__
    #include "io/EpollAsyncAutoResetEvent.hpp"
    #include "io/IOAsyncStdin.hpp"
    #include "io/IOContext.hpp"
    #include "io/IOOperation.hpp"
#endif

#endif // TINYCORO_TINYCORO_HPP
