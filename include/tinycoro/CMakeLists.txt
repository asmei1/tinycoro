project (TinyCoro LANGUAGES CXX)
add_library(TinyCoro FireAndForget.hpp ChronoLiteralsCoAwait.hpp StaticCoroThreadPool.hpp StaticCoroThreadPool.cpp TinyCoro.hpp Traits.hpp Task.hpp Generator.hpp Algoritms.hpp)

add_subdirectory(io)

target_link_libraries(TinyCoro io)
target_include_directories(TinyCoro PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
