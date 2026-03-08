cmake_minimum_required(VERSION 3.16)
project(raytracer CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(raytracer ${SOURCES})
target_include_directories(raytracer PRIVATE src)

if(MSVC)
    target_compile_options(raytracer PRIVATE /O2 /W4)
else()
    target_compile_options(raytracer PRIVATE -O3 -Wall -Wextra)
endif()
