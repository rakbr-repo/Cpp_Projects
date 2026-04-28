cmake_minimum_required(VERSION 3.10)

# Create a library target
add_library(JsonBuilder STATIC
    src/JsonBuilder.cpp
)

# Include headers
target_include_directories(JsonBuilder PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

find_package(nlohmann_json REQUIRED)

target_link_libraries(JsonBuilder
    PUBLIC CANReader
    PUBLIC HttpClient
    PUBLIC nlohmann_json::nlohmann_json
)
