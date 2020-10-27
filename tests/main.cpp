//
// Created by Asmei on 10/27/2020.
//

#include "tinycoro/FireAndForget.hpp"
#include <coroutine>
#include <iostream>

tinycoro::FireAndForget test() {
    co_await std::suspend_never{};
    std::cout << "Hello" << std::endl;
}

tinycoro::FireAndForget test2() {
    std::cout << "Hello" << std::endl;
}

int main() {
    auto a = test2();
    return 0;
}