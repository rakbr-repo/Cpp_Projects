cmake_minimum_required(VERSION 3.10)

# Create a library target
add_library(HttpClient STATIC
    src/HttpClient.cpp
)

# Include headers
target_include_directories(HttpClient PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

# Link libcurl (needed for HttpClient)
find_package(CURL REQUIRED)
target_link_libraries(HttpClient PUBLIC CURL::libcurl)