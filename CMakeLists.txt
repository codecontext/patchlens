cmake_minimum_required(VERSION 3.16)

project(PatchLens
    VERSION 0.1.0
    DESCRIPTION "A native Git patch viewer for Linux"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(PatchLens
    src/main.cpp
)

target_include_directories(PatchLens PRIVATE
    include
)
