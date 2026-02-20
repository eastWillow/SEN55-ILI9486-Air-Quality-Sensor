cmake_minimum_required(VERSION 3.10)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(EmulatorTests)
    
    # If standalone, we need to locate dependencies relative to this folder
    # Assuming standard repo layout
    set(PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/..")
    
    # Add GoogleTest if not already available
    if(NOT TARGET GTest::gtest_main)
        add_subdirectory(${PROJECT_ROOT}/external/googletest ${CMAKE_BINARY_DIR}/googletest)
    endif()
    
    # Build CoreLib in standalone mode
    # We need to build CoreLib to link against it in tests
    if(NOT TARGET CoreLib)
        # Find SDL2 for CoreLib
        find_package(SDL2 QUIET)
        
        # Define CoreLib sources (from parent CMakeLists.txt)
        set(LIB_SOURCES
            ${PROJECT_ROOT}/src/App/App.cpp
            ${PROJECT_ROOT}/src/App/EmulatorEngine.cpp
            ${PROJECT_ROOT}/src/Sensor/SensorMock.cpp
            ${PROJECT_ROOT}/src/Display/LCD_Driver_SDL.cpp
            ${PROJECT_ROOT}/src/Display/LCD_GUI.cpp
            ${PROJECT_ROOT}/src/Display/LCD_Touch.cpp
            ${PROJECT_ROOT}/src/Display/DEV_Config.cpp
            ${PROJECT_ROOT}/src/Fonts/font8.c
            ${PROJECT_ROOT}/src/Fonts/font12.c
            ${PROJECT_ROOT}/src/Fonts/font16.c
            ${PROJECT_ROOT}/src/Fonts/font20.c
            ${PROJECT_ROOT}/src/Fonts/font24.c
        )
        
        # Create the static library
        add_library(CoreLib STATIC ${LIB_SOURCES})
        
        # Configure include directories
        target_include_directories(CoreLib PUBLIC
            ${PROJECT_ROOT}/src/Display
            ${PROJECT_ROOT}/src/Fonts
            ${PROJECT_ROOT}/src/Sensor
            ${PROJECT_ROOT}/src/App
        )
        
        # Link SDL2
        if(SDL2_FOUND)
            target_include_directories(CoreLib PUBLIC ${SDL2_INCLUDE_DIRS})
            target_link_libraries(CoreLib PUBLIC ${SDL2_LIBRARIES})
        else()
            target_include_directories(CoreLib PUBLIC ${PROJECT_ROOT}/src/Display/include)
            # Create stub library if SDL2 is missing
            add_library(SDL2Stub ${PROJECT_ROOT}/src/Display/include/SDL2/SDL_stub.cpp)
            target_link_libraries(CoreLib PUBLIC SDL2Stub)
        endif()
    endif()
endif()

# Enable testing
enable_testing()

# Add GoogleTest
# Since external/googletest is in the project root, we refer to it using CMAKE_SOURCE_DIR
# Only add if NOT already added (handling the case where root added it, or we added it above)
if(NOT TARGET GTest::gtest_main)
    add_subdirectory(${CMAKE_SOURCE_DIR}/external/googletest ${CMAKE_BINARY_DIR}/googletest)
endif()

# Locate GTest
include(GoogleTest)

# Build the Unit Test executable
add_executable(emulator_test unit/emulator_test.cpp)

# Link GTest and CoreLib
target_link_libraries(emulator_test
    GTest::gtest_main
    CoreLib
)

# Register the unit test
gtest_discover_tests(emulator_test)

# Build the Performance Test executable
add_executable(performance_test unit/performance_test.cpp)

target_link_libraries(performance_test
    GTest::gtest_main
    CoreLib
)

gtest_discover_tests(performance_test)

# Build the Integration Test executable
add_executable(display_integration_test integration/display_integration_test.cpp)

target_link_libraries(display_integration_test
    GTest::gtest_main
    CoreLib
)

# Pass the reference directory location to the integration test
target_compile_definitions(display_integration_test PRIVATE
    TEST_RESOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/integration/reference_screenshots"
)

# Register the integration test
gtest_discover_tests(display_integration_test)
