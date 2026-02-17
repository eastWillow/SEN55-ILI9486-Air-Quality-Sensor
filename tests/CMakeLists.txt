# Enable testing
enable_testing()

# Add GoogleTest
# Since external/googletest is in the project root, we refer to it using CMAKE_SOURCE_DIR
add_subdirectory(${CMAKE_SOURCE_DIR}/external/googletest ${CMAKE_BINARY_DIR}/googletest)

# Locate GTest
include(GoogleTest)

# Build the GTest executable
add_executable(emulator_test emulator_test.cpp)

# Link GTest and the CoreLib (assuming it's available from parent scope)
target_link_libraries(emulator_test
    GTest::gtest_main
    CoreLib
)

# Register the test
# Copy reference images to build dir so test can find them at tests/reference/...
file(COPY reference DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/tests)

add_test(NAME EmulatorIntegrationTest COMMAND emulator_test)

# Discover tests
gtest_discover_tests(emulator_test)
