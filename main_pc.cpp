#include "src/App/App.h"
#include "src/Sensor/SensorMock.h"
#include "src/Display/LCD_Driver.h"
#include "src/Display/LCD_Driver_SDL.h"
#include <SDL2/SDL.h>

int main(int argc, char* argv[]) {
    SensorMock mockSensor;

    // Setup
    App_Setup(&mockSensor);

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
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                     SDL_SetMouseState(e.button.x, e.button.y, true);
                }
            } else if (e.type == SDL_MOUSEBUTTONUP) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                     SDL_SetMouseState(e.button.x, e.button.y, false);
                }
            } else if (e.type == SDL_MOUSEMOTION) {
                // Optionally update position while moving, though touch usually implies "touching" at a point
                // But for mouse simulation, we can just update position always, or only when dragging.
                // For simplicity, let's update position if we are considering it "touching" or just always update pos
                // but only set "pressed" on click.
                // However, touch screens usually only report coordinates when touched.
                // So we might only want to update coordinates when the button is held.
                // Let's stick to updating state on clicks for now, but if we drag, we want updates.
                if (e.motion.state & SDL_BUTTON_LMASK) {
                    SDL_SetMouseState(e.motion.x, e.motion.y, true);
                } else {
                    // Update cursor position even if not pressed?
                    // Touch screen: No touch -> no coordinates (or old ones).
                    // We'll just leave old coordinates but pressed=false.
                     SDL_SetMouseState(e.motion.x, e.motion.y, false);
                }
            }
        }
    }

    // Save screenshot
    LCD_SaveScreenshot("screenshot.bmp");

    // Cleanup
    LCD_Quit();

    return 0;
}
