cmake_minimum_required(VERSION 3.16)
project(CubisRenderer C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# --- TGA library ---
add_library(tga
    external/tga/tga.c
)

target_include_directories(tga PUBLIC
    external/tga
)

# --- Your executable ---
add_executable(CubisRenderer
    src/main.c
)

target_link_libraries(CubisRenderer PRIVATE tga)
