cmake_minimum_required(VERSION 3.10)

# Create a library target
add_library(CANReader STATIC
    src/CANReader.cpp
)

# Include headers
target_include_directories(CANReader PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

# Link libcurl (needed for CANReader)
find_package(CURL REQUIRED)
target_link_libraries(CANReader PUBLIC CURL::libcurl)