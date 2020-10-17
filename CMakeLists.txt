cmake_minimum_required(VERSION 3.12.4)

add_compile_definitions(basic_dotc_assembler)

file(GLOB_RECURSE SRC_FILES "assembler/src/*.cpp")

project(basic_dotc_assembler)

configure_file("assembler/include/bdc_assembler/in/config.hpp.in" "hello.hpp")

add_executable(basic_dotc_assembler ${SRC_FILES})

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

set_property(TARGET basic_dotc_assembler PROPERTY CXX_STANDARD 20)

include_directories("assembler/include")

target_link_libraries(basic_dotc_assembler ${CMAKE_DL_LIBS})
