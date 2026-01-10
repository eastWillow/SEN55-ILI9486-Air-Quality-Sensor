#include "src/App/App.h"
#include "src/Sensor/SensorMock.h"
#include "src/Display/LCD_Driver.h"
#include "src/Display/LCD_Driver_SDL.h"
#include <SDL2/SDL.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

// Global sensor instance
SensorMock mockSensor;

// The main loop function for Emscripten
void emscripten_main_loop() {
    App_Loop(&mockSensor);
    LCD_Update();

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            // For this simple case, we can just let the user close the tab.
        }
    }
}

int main(int argc, char* argv[]) {
    // Setup
    App_Setup(&mockSensor);

#ifdef __EMSCRIPTEN__
    // For the web, we set a persistent main loop
    emscripten_set_main_loop(emscripten_main_loop, 0, 1);
#else
    // Loop
    // For emulation, we might want to run for a few seconds or frames,
    // then take a screenshot and exit, or run interactively.
    // Given the task "generate the BMP file", we will run once, update, save, and exit.

    // We'll run the loop a few times to simulate data updates
    for (int i = 0; i < 3; i++) {
        App_Loop(&mockSensor);
        LCD_Update();

        // Handle SDL events to keep window responsive if visible
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                LCD_Quit();
                return 0;
            }
        }
    }

    // Save screenshot
    LCD_SaveScreenshot("screenshot.bmp");

    // Cleanup
    LCD_Quit();
#endif

    return 0;
}
