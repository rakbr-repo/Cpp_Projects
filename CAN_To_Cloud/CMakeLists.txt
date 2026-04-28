cmake_minimum_required(VERSION 3.10)
project(CANToCloud)

set(CMAKE_CXX_STANDARD 17)

# Add subdirectory for HttpClient library
add_subdirectory(CANReader)
add_subdirectory(JsonBuilder)
add_subdirectory(http)

# Build main executable
add_executable(CANToCloud main.cpp)

# Link library
target_link_libraries(CANToCloud
    PRIVATE HttpClient
    PRIVATE CANReader
    PRIVATE JsonBuilder
)