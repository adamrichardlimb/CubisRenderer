cmake_minimum_required(VERSION 3.16)
project(CubisRenderer C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(tga
    external/tga/tga.c
)

target_include_directories(tga PUBLIC
    external/tga
)

add_executable(CubisRenderer
    src/main.c
    src/framebuffer.c
    src/colour.c
)

target_link_libraries(CubisRenderer PRIVATE tga m)

target_compile_options(CubisRenderer PRIVATE
    $<$<CONFIG:Debug>:-O0 -g3>
    $<$<CONFIG:Release>:-Ofast -march=native -flto -DNDEBUG>
)

target_compile_definitions(CubisRenderer PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
)

target_link_options(CubisRenderer PRIVATE
    $<$<CONFIG:Release>:-flto>
)
