# Design: PM2.5 Chart Implementation

## Data Structures
To function within the Arduino Uno's strict 2KB RAM limit while providing useful history on PC:
```cpp
// App.h
#ifdef ARDUINO
#define CHART_MAX_POINTS 50
#else
#define CHART_MAX_POINTS 200
#endif

// App.cpp
static uint16_t chartData[CHART_MAX_POINTS];
static int chartCount = 0;
```
Using `uint16_t` allows storing PM2.5 values up to 65535, which covers the sensor range, while costing only 100 bytes on Arduino.

## Navigation Logic
The `App_Loop` state machine handles transitions:
- **Main Screen**: Detects touch on `BTN_TREND` area -> Sets `APP_STATE_CHART` -> Calls `DrawChartScreen`.
- **Chart Screen**: Detects touch on `BTN_BACK` area -> Sets `APP_STATE_MAIN` -> Calls `DrawMainScreen`.

## Rendering Algorithm
`DrawChartScreen` iterates through `chartData`:
1.  **Scaling**: Scans `chartData` to find `maxVal`. Y-axis pixels = `(value / maxVal) * chartHeight`.
2.  **Drawing**: Connects points $(x_i, y_i)$ to $(x_{i+1}, y_{i+1})$ using `GUI_DrawLine`.
3.  **Spacing**: $x$ step is calculated as `chartWidth / (CHART_MAX_POINTS - 1)` to fill the width.

## Test Infrastructure Improvements
To validate this visual feature in CI:
1.  **Input Simulation**: Added `EmulatorEngine::setMouseState` to simulate clicks on the new Trend button.
2.  **BMP Fix**: The standard `SDL_SaveBMP` produced 16-bit images incompatible with some CI ImageMagick versions. A custom `WriteBMP` function was implemented in `LCD_Driver_SDL.cpp` to write standard 24-bit RGB headers and data, ensuring consistent behavior between local (stubbed SDL) and CI (real SDL) environments.
