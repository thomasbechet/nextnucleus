cmake_minimum_required(VERSION 3.5.0)

project(tests LANGUAGES C VERSION 0.0.1)

include_directories(../)

set(CMAKE_BUILD_TYPE Debug)

add_executable(indent ident.c)
add_executable(list list.c)
add_executable(slotmap slotmap.c)

add_test(indent indent)
add_test(list list)
add_test(slotmap slotmap)
