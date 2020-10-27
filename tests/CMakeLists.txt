add_executable (TinyCoroTests main.cpp)
target_link_libraries(TinyCoroTests
        TinyCoro
        )

# add the binary tree to the search path for include files
target_include_directories(TinyCoroTests PUBLIC
        "${CMAKE_SOURCE_DIR}"
        "${CMAKE_SOURCE_DIR}/include/tinycoro"
        )

set_target_properties(TinyCoro PROPERTIES LINKER_LANGUAGE CXX)
