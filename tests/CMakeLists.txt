cmake_minimum_required(VERSION 3.10)
project(UnitTests)

set(CMAKE_CXX_STANDARD 11)

# Add GoogleTest
# We assume it is at ../external/googletest
add_subdirectory(${CMAKE_SOURCE_DIR}/external/googletest external/googletest)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# Helper to find CoreLib headers
include_directories(../src/Display ../src/App ../src/Sensor ../src/Fonts)

set(TEST_SOURCES
    test_main.cpp
    test_sensor_disconnect.cpp
)

add_executable(UnitTests ${TEST_SOURCES})

target_link_libraries(UnitTests CoreLib gtest gmock)
