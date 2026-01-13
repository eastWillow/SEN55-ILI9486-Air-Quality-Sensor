cmake_minimum_required(VERSION 3.10)
project(DisplayEmulator)

set(CMAKE_CXX_STANDARD 11)

if(EMSCRIPTEN)
    add_compile_options(-s USE_SDL=2)
    add_link_options(-s USE_SDL=2)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
else()
    # Check if SDL2 is found normally
    find_package(SDL2 QUIET)
endif()

# Define Core Sources
set(CORE_SOURCES
    src/App/App.cpp
    src/Display/LCD_Driver_SDL.cpp
    src/Display/LCD_GUI.cpp
    src/Display/LCD_Touch.cpp
    src/Display/DEV_Config.cpp
    src/Fonts/font8.c
    src/Fonts/font12.c
    src/Fonts/font16.c
    src/Fonts/font20.c
    src/Fonts/font24.c
)

add_library(CoreLib STATIC ${CORE_SOURCES})

# Config Includes for CoreLib
target_include_directories(CoreLib PUBLIC
    src/Display
    src/Fonts
    src/Sensor
    src/App
)

if(NOT EMSCRIPTEN)
    if(SDL2_FOUND)
        target_include_directories(CoreLib PUBLIC ${SDL2_INCLUDE_DIRS})
        target_link_libraries(CoreLib PUBLIC ${SDL2_LIBRARIES})
    else()
        message(STATUS "SDL2 not found, using local mock/include for compilation check")
        target_include_directories(CoreLib PUBLIC src/Display/include)

        # Stub logic
        add_library(SDL2Stub src/Display/include/SDL2/SDL_stub.cpp)
        target_link_libraries(CoreLib PUBLIC SDL2Stub)
    endif()
endif()

# Main PC Emulator
set(EMULATOR_SOURCES
    main_pc.cpp
    src/Sensor/SensorMock.cpp
)

add_executable(DisplayEmulator ${EMULATOR_SOURCES})
target_link_libraries(DisplayEmulator CoreLib)

if(EMSCRIPTEN)
    set_target_properties(DisplayEmulator PROPERTIES OUTPUT_NAME "index")
    target_link_options(DisplayEmulator PUBLIC "-s USE_SDL=2")
endif()

# Tests
option(BUILD_TESTING "Build tests" ON)
if(BUILD_TESTING AND NOT EMSCRIPTEN)
    add_subdirectory(tests)
endif()
