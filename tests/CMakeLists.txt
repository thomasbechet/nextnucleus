cmake_minimum_required(VERSION 3.5.0)

project(tests LANGUAGES C VERSION 0.0.1)

include_directories(../)

add_executable(indent ident.c)
add_executable(memory memory.c)
add_executable(mmap mmap.c)

add_test(indent indent)
add_test(memory memory)
add_test(mmap mmap)
