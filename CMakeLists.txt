cmake_minimum_required(VERSION 3.10)
project(DisplayEmulator)

set(CMAKE_CXX_STANDARD 11)
enable_testing()

if(EMSCRIPTEN)
    add_compile_options(-s USE_SDL=2)
    add_link_options(-s USE_SDL=2)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
else()
    # Check if SDL2 is found normally
    find_package(SDL2 QUIET)

    if(SDL2_FOUND)
        include_directories(${SDL2_INCLUDE_DIRS})
    else()
        message(STATUS "SDL2 not found, using local mock/include for compilation check")
        include_directories(src/Display/include)
    endif()
endif()

include_directories(src/Display src/Fonts src/Sensor src/App)

# Core Source files (excluding main entry point)
set(CORE_SOURCES
    src/App/App.cpp
    src/Sensor/SensorMock.cpp
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

# Handle dependencies for CoreLib
if(EMSCRIPTEN)
    target_link_options(CoreLib PUBLIC "-s USE_SDL=2")
else()
    if(SDL2_FOUND)
        target_link_libraries(CoreLib ${SDL2_LIBRARIES})
    else()
        # Mock linking if SDL is missing
        add_library(SDL2Stub src/Display/include/SDL2/SDL_stub.cpp)
        target_link_libraries(CoreLib SDL2Stub)
    endif()
endif()

# Main Emulator Executable
add_executable(DisplayEmulator main_pc.cpp)
target_link_libraries(DisplayEmulator CoreLib)

if(EMSCRIPTEN)
    set_target_properties(DisplayEmulator PROPERTIES OUTPUT_NAME "index")
    target_link_options(DisplayEmulator PUBLIC "-s USE_SDL=2")
else()
    # Dependencies are transitive from CoreLib, but good to be explicit if needed.
    # Actually, static libs propagate dependencies usually if PUBLIC/INTERFACE used,
    # but here we used target_link_libraries(CoreLib ...) which defaults to PUBLIC for static libs in some versions?
    # No, default is PUBLIC for static libs only if specified?
    # CMake < 3.13 default is public? No.
    # Let's rely on transitive linking.
endif()

# Add Testing
if(NOT EMSCRIPTEN)
    add_subdirectory(tests)
endif()
