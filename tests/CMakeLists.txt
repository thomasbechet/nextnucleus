cmake_minimum_required(VERSION 3.5.0)

project(tests LANGUAGES C VERSION 0.0.1)

include_directories(../)

set(CMAKE_BUILD_TYPE Debug)

add_definitions(-DNU_DEBUG)
add_definitions(-DNU_STDLIB)

add_executable(string string.c)
add_executable(list list.c)
add_executable(slotmap slotmap.c)

add_test(string string)
add_test(list list)
add_test(slotmap slotmap)
