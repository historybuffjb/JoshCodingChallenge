cmake_minimum_required(VERSION 3.14)
project(light_monitor LANGUAGES CXX)

find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(light_monitor
    src/main.cpp
    src/HttpClient.cpp
    src/Backoff.cpp
    src/LightsClient.cpp
    src/ConsoleReporter.cpp
    src/LightMonitor.cpp
)

target_include_directories(light_monitor PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(light_monitor PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(light_monitor PRIVATE Threads::Threads)
