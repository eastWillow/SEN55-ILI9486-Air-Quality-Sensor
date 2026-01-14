# Enable testing for this directory
enable_testing()

# Add GoogleTest
# Exclude gtest's own tests from the build
set(gtest_build_tests OFF CACHE BOOL "GTest build tests" FORCE)
set(gtest_build_samples OFF CACHE BOOL "GTest build samples" FORCE)
set(gmock_build_tests OFF CACHE BOOL "GMock build tests" FORCE)

add_subdirectory(${CMAKE_SOURCE_DIR}/external/googletest ${CMAKE_BINARY_DIR}/googletest)

# Define the test executable
add_executable(UnitTests
    AppTests.cpp
    # Add other test files here
)

# Link against CoreLib and GTest/GMock
target_link_libraries(UnitTests
    PRIVATE
    CoreLib
    gtest
    gtest_main
    gmock
)

# Ensure UnitTests can find headers in CoreLib source directories
target_include_directories(UnitTests PRIVATE
    ${CMAKE_SOURCE_DIR}/src/App
    ${CMAKE_SOURCE_DIR}/src/Sensor
    ${CMAKE_SOURCE_DIR}/src/Display
    ${CMAKE_SOURCE_DIR}/tests/mocks
)

# Discover tests (requires CMake 3.10+)
include(GoogleTest)
gtest_discover_tests(UnitTests)
