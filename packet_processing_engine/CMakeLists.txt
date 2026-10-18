cmake_minimum_required(VERSION 3.16)
project(packet_processing_engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wpedantic -Wextra)

add_executable(packet_server
    main.cpp
    src/network/connection.cpp
    src/network/tcp_server.cpp
)

target_include_directories(packet_server PRIVATE include)

add_executable(packet_client
    tools/client.cpp
)
