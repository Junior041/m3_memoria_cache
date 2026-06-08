cmake_minimum_required(VERSION 3.10)
project(m3_memoria_cache)

set(CMAKE_CXX_STANDARD 20)

add_executable(m3_memoria_cache
    src/main.cpp
    src/cache.cpp
    src/address_reader.cpp
    src/display.cpp
)

# Permite que os #include "cache.h" etc. funcionem dentro de src/
target_include_directories(m3_memoria_cache PRIVATE src)
