cmake_minimum_required(VERSION 3.10)
project(DisplayEmulator)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(EMSCRIPTEN)
    add_compile_options(-s USE_SDL=2)
    add_link_options(-s USE_SDL=2)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
else()
    # Check if SDL2 is found normally
    find_package(SDL2 QUIET)
    if(NOT SDL2_FOUND)
        message(STATUS "SDL2 not found, using local mock/include for compilation check")
    endif()
endif()

# Define source files for the library (everything except main_pc.cpp)
set(LIB_SOURCES
    src/App/App.cpp
    src/App/EmulatorEngine.cpp
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

# Create the static library
add_library(CoreLib STATIC ${LIB_SOURCES})

# Configure include directories for the library and its consumers
target_include_directories(CoreLib PUBLIC
    src/Display
    src/Fonts
    src/Sensor
    src/App
)

if(EMSCRIPTEN)
    # Emscripten handles SDL via flags which are already added globally or can be target specific
    # We added global options above, but let's ensure CoreLib compiles correctly
else()
    if(SDL2_FOUND)
        target_include_directories(CoreLib PUBLIC ${SDL2_INCLUDE_DIRS})
        target_link_libraries(CoreLib PUBLIC ${SDL2_LIBRARIES})
    else()
        target_include_directories(CoreLib PUBLIC src/Display/include)
        # Create stub library if SDL2 is missing
        add_library(SDL2Stub src/Display/include/SDL2/SDL_stub.cpp)
        target_link_libraries(CoreLib PUBLIC SDL2Stub)
    endif()
endif()

# Create the executable
add_executable(DisplayEmulator main_pc.cpp)

# Link the executable against CoreLib
target_link_libraries(DisplayEmulator PRIVATE CoreLib)

if(NOT EMSCRIPTEN)
    enable_testing()
    add_subdirectory(tests)
endif()

if(EMSCRIPTEN)
    set_target_properties(DisplayEmulator PROPERTIES OUTPUT_NAME "index")
    target_link_options(DisplayEmulator PUBLIC "-s USE_SDL=2")
endif()
