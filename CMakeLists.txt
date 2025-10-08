cmake_minimum_required(VERSION 3.10)
project(simple_deb_packager)
set(CMAKE_CXX_STANDARD 17)

add_executable(simple_deb_packager src/main.cpp)
target_link_libraries(simple_deb_packager readline)
