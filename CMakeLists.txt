cmake_minimum_required(VERSION 3.21)
project(botnet_client)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

add_executable(bitwi2dl.exe main.cpp inc/getosname.h inc/json.hpp)
set_target_properties(bitwi2dl.exe PROPERTIES LINK_SEARCH_START_STATIC 1)
target_link_libraries(bitwi2dl.exe "-lwsock32 -lws2_32 -lcurl/curl")
set_target_properties(bitwi2dl.exe PROPERTIES LINK_SEARCH_END_STATIC 1)

add_executable(bitwi2dl main.cpp inc/getosname.h inc/json.hpp)
target_link_libraries(bitwi2dl "-lcurl")
