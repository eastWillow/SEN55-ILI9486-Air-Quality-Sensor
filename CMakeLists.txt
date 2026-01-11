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

    if(SDL2_FOUND)
        include_directories(${SDL2_INCLUDE_DIRS})
    else()
        message(STATUS "SDL2 not found, using local mock/include for compilation check")
        include_directories(src/Display/include)
    endif()
endif()

include_directories(src/Display src/Fonts src/Sensor src/App)

# Source files
set(SOURCES
    main_pc.cpp
    src/App/App.cpp
    src/Sensor/SensorMock.cpp
    src/Display/LCD_Driver_SDL.cpp
    src/Display/LCD_GUI.cpp
    src/Display/DEV_Config.cpp
    src/Fonts/font8.c
    src/Fonts/font12.c
    src/Fonts/font16.c
    src/Fonts/font20.c
    src/Fonts/font24.c
)

add_executable(DisplayEmulator ${SOURCES})

if(EMSCRIPTEN)
    target_link_options(DisplayEmulator PUBLIC "-s USE_SDL=2")
else()
    if(SDL2_FOUND)
        target_link_libraries(DisplayEmulator ${SDL2_LIBRARIES})
    else()
        # Mock linking if SDL is missing (compilation only, linking will fail if symbols missing)
        # Actually, we need to stub the symbols if we want to link.
        # Let's create a stub source file.
        add_library(SDL2Stub src/Display/include/SDL2/SDL_stub.cpp)
        target_link_libraries(DisplayEmulator SDL2Stub)
    endif()
endif()
