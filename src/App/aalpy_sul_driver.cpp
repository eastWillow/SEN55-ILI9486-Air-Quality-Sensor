#ifndef ARDUINO

#include "App.h"
#include "EmulatorEngine.h"
#include "DEV_Config.h"
#include "LCD_Driver.h"
#include "LCD_Driver_SDL.h"
#include "LCD_Touch.h"
#include "SensorMock.h"
#include <cstdlib>
#include <iostream>
#include <string>

#ifdef __EMSCRIPTEN__
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

// Global instances
static SensorMock mockSensor;
static SystemTimeProvider sysTime;
static bool isInitialized = false;

static void EnsureInitialized() {
  if (!isInitialized) {
    SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "1");
    srand(1);
    mockSensor.setSeed(1);
    System_SetFastMode(true);
    App_Setup(&mockSensor, &sysTime);
    isInitialized = true;
  }
}

static void SimulateClick(uint16_t x, uint16_t y) {
  // Set internal touch mock mouse state and dispatch event
  SDL_SetMouseState(x, y, true);

  SDL_Event event;
  SDL_memset(&event, 0, sizeof(event));
  event.type = SDL_MOUSEBUTTONDOWN;
  event.button.button = SDL_BUTTON_LEFT;
  event.button.x = x;
  event.button.y = y;
  event.button.state = SDL_PRESSED;
  SDL_PushEvent(&event);

  // Process event in App_Loop
  App_Loop(&mockSensor);
  LCD_Update();

  // Step 2 frames to ensure feedback / touch scan registers
  for (int i = 0; i < 2; ++i) {
    App_Loop(&mockSensor);
    LCD_Update();
  }

  // Mouse button up
  SDL_SetMouseState(x, y, false);

  SDL_memset(&event, 0, sizeof(event));
  event.type = SDL_MOUSEBUTTONUP;
  event.button.button = SDL_BUTTON_LEFT;
  event.button.x = x;
  event.button.y = y;
  event.button.state = SDL_RELEASED;
  SDL_PushEvent(&event);

  // Step 3 frames (150ms simulated time) to complete button press transition (debounce requires 100ms)
  for (int i = 0; i < 3; ++i) {
    App_Loop(&mockSensor);
    LCD_Update();
  }
}

static void SimulateTick(int frames) {
  for (int i = 0; i < frames; ++i) {
    App_Loop(&mockSensor);
    LCD_Update();
  }
}

// cppcheck-suppress constParameter
int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <action> [args...]" << std::endl;
    std::cerr << "Actions:" << std::endl;
    std::cerr << "  reset" << std::endl;
    std::cerr << "  click <x> <y>" << std::endl;
    std::cerr << "  tick <frames>" << std::endl;
    std::cerr << "  state" << std::endl;
    std::cerr << "  screenshot <filename>" << std::endl;
    return 1;
  }

  EnsureInitialized();

  // Command-line driver for persistent state machine execution or sequence execution
  int argIdx = 1;
  while (argIdx < argc) {
    std::string cmd = argv[argIdx++];
    if (cmd == "reset") {
      App_ResetState();
      App_Setup(&mockSensor, &sysTime);
      // Run 5 warm-up frames
      SimulateTick(5);
    } else if (cmd == "click") {
      if (argIdx + 1 < argc) {
        uint16_t x = static_cast<uint16_t>(std::atoi(argv[argIdx++]));
        uint16_t y = static_cast<uint16_t>(std::atoi(argv[argIdx++]));
        SimulateClick(x, y);
      }
    } else if (cmd == "tick") {
      if (argIdx < argc) {
        int frames = std::atoi(argv[argIdx++]);
        SimulateTick(frames);
      }
    } else if (cmd == "state") {
      AppState st = App_GetState();
      switch (st) {
      case APP_STATE_MAIN:
        std::cout << "MAIN" << std::endl;
        break;
      case APP_STATE_INFO:
        std::cout << "INFO" << std::endl;
        break;
      case APP_STATE_TREND:
        std::cout << "TREND" << std::endl;
        break;
      default:
        std::cout << "UNKNOWN" << std::endl;
        break;
      }
    } else if (cmd == "screenshot") {
      if (argIdx < argc) {
        std::string path = argv[argIdx++];
        LCD_SaveScreenshot(path.c_str());
        std::cout << "SAVED:" << path << std::endl;
      }
    }
  }

  LCD_Quit();
  return 0;
}

#endif
