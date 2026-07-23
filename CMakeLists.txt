cmake_minimum_required(VERSION 3.16)

project(PatchLens
    VERSION 0.1
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

add_executable(PatchLens
    src/main.cpp
    src/ui/mainwindow.cpp
    src/core/patch/patchparser.cpp
)

target_include_directories(PatchLens
    PRIVATE
        src
        src/core
        src/ui
)

target_link_libraries(PatchLens
    PRIVATE
        Qt6::Widgets
)
