project (TinyCoro LANGUAGES CXX)
add_library(TinyCoro FireAndForget.hpp ChronoLiteralsCoAwait.hpp StaticCoroThreadPool.hpp StaticCoroThreadPool.cpp TinyCoro.hpp Traits.hpp Task.hpp Generator.hpp Algoritms.hpp io/IOContext.hpp io/File.cpp io/File.h)
include_directories(io)
