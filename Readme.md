# tinycoro
It's a simple library which supports a coroutine usage and implemented a some coroutine types (like Task<T> or Generator<T>, some Linux-specific types and context classes for all of them.  
This library used a C++20 standard.

## Usage
#Prerequisites
You will need:
 - A gcc compiler which supports coroutines (10+)
 - Linux system for tinycoro::io part

## Build and install
```
mkdir src/build
cd src/build
cmake .. 
cmake --build
```

Installation using cmake is not done yet.

## Details

### `tinycoro::Task<T>`
Coroutine which will be not executed until it won't be called with co_await.  
Coroutine body which will returns a `tiny::Task<T>` have to contains the `co_await` or `co_return` keyword.
`co_yield` is forbidden here.
Until coroutine will be not executed with co_await, it will be suspend in awaiting state.
It will be resumed later on thread where it will be co_await'ed.  
Any exception will stored and rethrow in resume thread.  
If `tiny::Task<T>` object was destroyed before coroutine function was executed, all memory is frees automatically.  

```c++
// If this function will be co_awaited, it will be resumed with scheduler on specific context (ie thread pool)
// Returns a result of computations
tinycoro::Task<int> someAsyncTask(tinycoro::scheduler_trait auto scheduler)
{
    co_await scheduler.schedule();
    
    // some computations
    uint64_t result = 0;
    for(auto i : tinycoro::range(0, 100000))
    {
        result += i;
    }

    co_return result;
};
```

### `tinycoro::Generator<T>`
Coroutine type which supports `co_yield` keyword. Values are produced lazy and synchronously.  
Coroutine body cannot use `co_await` keyword.  
Any exception will be propagate to the user by `begin()` or `operator++()` functions.  

```c++
//Example is the one of algorithms from Algorithms.hpp
//It could take any type which is addatable with Step type,
//ie. it could be any iterator.
template <typename T, typename Step>
requires requires (T x, Step s) { x + s; }
tinycoro::Generator<T> range(T begin, T end, Step step)
{
    while(begin < end)
    {
        co_yield begin;
        begin += step;
    }
}
//...
std::vector<int> v = {1, 2, 3, 4};
for(const auto& it : range(v.begin(), v.end(), 2)
{
    std::cout << *it << "\n";
}
```

### `tinycoro::StaticCoroThreadPool`
StaticCoroThreadPool is a simple static thread pool class, works as context for coroutines.  
It could schedule coroutine on the one of thread or returns a lightweight Scheduler object, which can do the same thing.  
Both, `tinycoro::StaticCoroThreadPool` and `tinycoro::StaticCoroThreadPool::Scheduler` fits a `tinycoro::schedule_trait` which could be used as template concept. 
  


### `tinycoro::FireAndForget`
Has the `co_await` operator deleted, so the returned result cannot be `co_await`ed.
``Fire and forget``, you are not interested in its result or whether it has managed to do something.   
It will be resumed immediatelly after call.  
Cannot create object explicit - the only possibility to use this type is as coroutine return type.   
Any exception will terminate application.

```c++
tinycoro::FireAndForget someAsyncTask()
{
    // do some task, but remember, any exception will terminate application!
    
    // cannot return
};
```


## Linux part

### `tinycoro::io::IOContext`
IOContext is the class for scheduling and invoking IO events, based on epoll_event mechanism from Linux.
Its context class for coroutines from `tinycoro::io`
All classes which are derived from `tinycoro::io::IOOperation` class needs this context.
For more information, look in `tinycoro::io::IOContext.hpp` 
```c++
tinycoro::FireAndForget waitUntilEventIsSet(tinycoro::io::EpollAsyncAutoResetEvent& e)
{
    co_await e;
    // do something
}
//..
{
    constexpr int MAX_EVENTS = 4;
    tinycoro::io::IOContext ioContext{MAX_EVENTS};
    tinycoro::io::EpollAsyncAutoResetEvent event{ioContext};

    waitUntilEventIsSet(event);
    
    while(true)
    {
        ioContext.processAwaitingEvents();    
    }
}
    
```


### `tinycoro::io::EpollAsyncAutoResetEvent`
A simple class with set and auto clear mechanism, that allow to wait one thread
until an event is signalled by a thread calling a set() function.  
State set is automatically set to "not set" state after wake up thread.  
Any exception will stored and rethrow in resume thread.  

```c++
tinycoro::Task<uint64_t> waitUntilEventIsSet(tinycoro::io::EpollAsyncAutoResetEvent& e)
{
    co_await e;
    co_return 256;
}

{
    tinycoro::io::IOContext context{10};  
    tinycoro::io::EpollAsyncAutoResetEvent event{context};
    waitUntilEventIsSet(event);
    
    event.set();
}

```

### `tinycoro::io::AsyncStdin`
AsyncStdin supports reading standard input stream with coroutine.
```c++
tinycoro::FireAndForget readFromStdin(tinycoro::io::IOContext& context)
{
    constexpr int READ_SIZE = 10;
    int eventCount = 0;
    size_t bytesRead = 0;
    char readBuffer[READ_SIZE + 1];

    tinycoro::io::AsyncStdin stdinCoro{context};
    while(true)
    {
        try
        {
            bytesRead = co_await stdinCoro.readData(readBuffer, READ_SIZE);
            readBuffer[bytesRead] = '\0';
            std::cout << "DATA: " << readBuffer << std::endl;
        }
        catch(...){}
    }    
}
```

#

----------------------------------------------------------------
The code is inspired by articles by Lewiss Baker and his [cppcoro](https://github.com/lewissbaker/cppcoro) and code from [luncliff](https://github.com/luncliff/coroutine/).
   Thanks!
