#ifndef ARDUINO

#include "src/App/App.h"
#include "src/Sensor/SensorMock.h"
#include "src/Display/LCD_Driver.h"
#include "src/Display/LCD_Driver_SDL.h"

#ifdef __EMSCRIPTEN__
#include <SDL.h>
#include <emscripten.h>
#else
#include <SDL2/SDL.h>
#endif

// Global sensor instance
SensorMock mockSensor;

// Shared input handling logic for both Emscripten and Native
void HandleInput(const SDL_Event& e) {
    if (e.type == SDL_MOUSEBUTTONDOWN) {
        if (e.button.button == SDL_BUTTON_LEFT) {
             SDL_SetMouseState(e.button.x, e.button.y, true);
        }
    } else if (e.type == SDL_MOUSEBUTTONUP) {
        if (e.button.button == SDL_BUTTON_LEFT) {
             SDL_SetMouseState(e.button.x, e.button.y, false);
        }
    } else if (e.type == SDL_MOUSEMOTION) {
        // Update position. If dragging (button held), update as pressed.
        // Otherwise, just update position (hover) with pressed=false.
        if (e.motion.state & SDL_BUTTON_LMASK) {
            SDL_SetMouseState(e.motion.x, e.motion.y, true);
        } else {
             SDL_SetMouseState(e.motion.x, e.motion.y, false);
        }
    }
}

// The main loop function for Emscripten
void emscripten_main_loop() {
    App_Loop(&mockSensor);
    LCD_Update();

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            // For this simple case, we can just let the user close the tab.
        } else {
            HandleInput(e);
        }
    }
}

int main(int argc, char* argv[]) {
    // Explicitly convert touch events to mouse events (standard SDL behavior, but enforced here)
    SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "1");

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
    // With 50ms simulated time per loop, 25 loops = 1.25s, enough to trigger the 1s sensor update.
    for (int i = 0; i < 25; i++) {
        App_Loop(&mockSensor);
        LCD_Update();

        // Handle SDL events to keep window responsive if visible
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                LCD_Quit();
                return 0;
            } else {
                HandleInput(e);
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
#endif
