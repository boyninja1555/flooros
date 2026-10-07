cmake_minimum_required(VERSION 3.3...4.2)
project(gooner LANGUAGES C)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Executable
add_executable(gooner)
target_sources(gooner PRIVATE src/main.c src/gnr/gnr.c src/gnr/gnrmake.c src/desktop/main.c src/desktop/fb.c src/desktop/kb.c src/desktop/image.c)
target_include_directories(gooner PRIVATE include)