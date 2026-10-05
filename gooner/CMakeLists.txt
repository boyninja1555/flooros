cmake_minimum_required(VERSION 3.3)
project(gooner LANGUAGES C)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Executable
add_executable(gooner)
target_sources(gooner PRIVATE src/main.c src/gnr.c)
target_include_directories(gooner PRIVATE include)