cmake_minimum_required(VERSION 3.10)
project(DisplayEmulator)

set(CMAKE_CXX_STANDARD 11)

# --- Dependency Setup (SDL2) ---
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

# Include project directories
include_directories(src/Display src/Fonts src/Sensor src/App)

# --- CoreLib Definition ---
# Shared sources between Emulator and Tests
set(SOURCES_CORE
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

add_library(CoreLib STATIC ${SOURCES_CORE})

# Handle dependencies for CoreLib
if(NOT EMSCRIPTEN)
    if(SDL2_FOUND)
        # If SDL2 is found, link it to CoreLib if possible, or just propagate usage
        target_link_libraries(CoreLib PUBLIC ${SDL2_LIBRARIES})
    else()
        # Mock linking if SDL is missing
        add_library(SDL2Stub src/Display/include/SDL2/SDL_stub.cpp)
        target_link_libraries(CoreLib PUBLIC SDL2Stub)
    endif()
else()
    # For Emscripten, flags are global or on target
endif()

# --- DisplayEmulator Executable ---
set(SOURCES_EMULATOR
    main_pc.cpp
)

add_executable(DisplayEmulator ${SOURCES_EMULATOR})
target_link_libraries(DisplayEmulator CoreLib)

if(EMSCRIPTEN)
    set_target_properties(DisplayEmulator PROPERTIES OUTPUT_NAME "index")
    target_link_options(DisplayEmulator PUBLIC "-s USE_SDL=2")
endif()

# --- Testing ---
# Only add tests if the directory exists (it should, based on our plan)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt")
    enable_testing()
    add_subdirectory(tests)
endif()
