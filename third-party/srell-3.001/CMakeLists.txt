cmake_minimum_required(VERSION 3.1)
project(srell VERSION 3.001)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a")

add_library(srell INTERFACE)

target_include_directories(
    srell
    INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
    "$<INSTALL_INTERFACE:include>"
)

set(HEADER_BASE "${CMAKE_CURRENT_SOURCE_DIR}/include")

### Install ###
set(include_install_dir "include")
set(targets_export_name "${PROJECT_NAME}Targets")
set(namespace "${PROJECT_NAME}::")

install(
    TARGETS srell
    EXPORT "${targets_export_name}"
    INCLUDES DESTINATION "${include_install_dir}"
)

install(DIRECTORY "include/srell" DESTINATION "${include_install_dir}")

file(GLOB_RECURSE srell_include_SRCS "${HEADER_BASE}/*.hpp")
add_custom_target(srell_headers_for_ide SOURCES ${srell_include_SRCS})