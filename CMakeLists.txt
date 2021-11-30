cmake_minimum_required(VERSION 3.0.0)
project(boolean_search)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

## srell
add_subdirectory(third-party/srell-3.001)

#include(ECMEnableSanitizers)

add_subdirectory(src)
