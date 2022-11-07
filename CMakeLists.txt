cmake_minimum_required(VERSION 3.21)
project(botnet_client)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

add_executable(bitwi2dl main.cpp inc/getosname.h inc/json.hpp)
target_link_libraries(bitwi2dl "-lcurl")