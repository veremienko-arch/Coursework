cmake_minimum_required(VERSION 4.2)
project(Masyu)

set(CMAKE_CXX_STANDARD 26)

set(CMAKE_PREFIX_PATH "/home/rostyslav/Qt/6.11.0/gcc_64")

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(test main.cpp)
target_link_libraries(test PRIVATE Qt6::Widgets)