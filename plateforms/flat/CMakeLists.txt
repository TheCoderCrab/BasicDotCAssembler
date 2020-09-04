cmake_minimum_required(VERSION 3.12.4) 

project(flat)

file(GLOB_RECURSE SRC_FILES "src/*.cpp")

add_library(flat SHARED ${SRC_FILES})

if(DEBUG)
    if(GDB)
        add_compile_definitions(GDB)
    endif(GDB)
    message("Building in debug mode")
    set(CMAKE_BUILD_TYPE Debug)
    add_compile_definitions(DEBUG)
else()
    message("Building in release mode")
    set(CMAKE_BUILD_TYPE Release)
    add_compile_definitions(RELEASE)
endif(DEBUG)


set_property(TARGET flat PROPERTY CXX_STANDARD 20)

include_directories("../include/")

target_link_libraries(flat)