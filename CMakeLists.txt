cmake_minimum_required(VERSION 3.16)
project(demo)

set(CMAKE_CXX_STANDARD 20)

add_executable(demo main.cpp Window.cpp Window.h App.cpp App.h Surface.cpp Surface.h)

TARGET_LINK_LIBRARIES(demo
                      SDL2)