cmake_minimum_required(VERSION 3.0.0)
project(hairpoll)

add_library(hairpoll SHARED hairpoll.cpp)
target_compile_options(hairpoll PRIVATE -std=c++1y)
target_compile_options(hairpoll PRIVATE -fvisibility=hidden)

add_executable(poller poller.cpp)
target_compile_options(poller PRIVATE -std=c++1y)
target_link_libraries(poller hairpoll)
