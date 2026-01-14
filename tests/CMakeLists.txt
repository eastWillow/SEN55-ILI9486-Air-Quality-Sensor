# Define the test suite
project(DisplayEmulatorTests)

# Add GoogleTest submodule
add_subdirectory(${CMAKE_SOURCE_DIR}/external/googletest ${CMAKE_BINARY_DIR}/googletest)

# Define the test executable
add_executable(unit_tests
    test_main.cpp
)

# Link against CoreLib and GoogleTest
target_link_libraries(unit_tests
    CoreLib
    GTest::gtest_main
    GTest::gmock
)
