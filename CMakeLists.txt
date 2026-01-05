cmake_minimum_required(VERSION 3.15)
project(raycast)

set(CMAKE_C_STANDARD 17)

find_package(SDL3 REQUIRED)

add_executable(raycast src/main.c)

target_link_libraries(raycast PRIVATE SDL3::SDL3 m imgui)