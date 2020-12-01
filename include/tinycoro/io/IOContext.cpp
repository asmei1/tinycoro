//
// Created by Asmei on 11/27/2020.
//
#include "IOContext.hpp"
using namespace tinycoro::io;

void IOContext::processAwaitingEvents()
{

}

IOContext::IOContextOperation IOContext::schedule()
{
    return {*this};
}
