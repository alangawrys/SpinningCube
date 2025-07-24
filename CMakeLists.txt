cmake_minimum_required(VERSION 3.31)
project(SnippingCube C)

set(CMAKE_C_STANDARD 23)

add_executable(SnippingCube spinCube.c)

if(UNIX)
    target_link_libraries(SnippingCube m)
endif()