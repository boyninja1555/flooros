cmake_minimum_required(VERSION 3.3...4.2)
project(gooner LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Executable
add_executable(gooner)
target_sources(gooner PRIVATE src/main.cpp src/image.cpp src/fb.cpp src/glyph.cpp src/kb.cpp)
target_include_directories(gooner PRIVATE include)