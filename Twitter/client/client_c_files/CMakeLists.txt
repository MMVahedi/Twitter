cmake_minimum_required(VERSION 3.17)
project(client_c_files C)

set(CMAKE_C_STANDARD 99)

add_executable(client_c_files main.c slre.c)
target_link_libraries(client_c_files ws2_32)