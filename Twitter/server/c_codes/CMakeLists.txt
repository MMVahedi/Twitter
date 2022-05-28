cmake_minimum_required(VERSION 3.17)
project(c_codes C)

set(CMAKE_C_STANDARD 99)

add_executable(c_codes main.c slre.c)
target_link_libraries(c_codes ws2_32)