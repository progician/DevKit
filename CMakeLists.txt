cmake_minimum_required(VERSION 3.10)

list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(catch2 REQUIRED)
find_package(trompeloeil REQUIRED)

set(TOP_LEVEL_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include)

add_subdirectory(lib)
add_subdirectory(tools)
