add_executable (TinyCoroTests main.cpp)
target_link_libraries(TinyCoroTests TinyCoro)

set_target_properties(TinyCoro PROPERTIES LINKER_LANGUAGE CXX)
