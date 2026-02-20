## Why

Users need to understand not just the *current* air quality, but how it is changing. A sudden spike in PM2.5 requires different action than a slow accumulation. Providing a historical trend chart allows users to diagnose sources (e.g., "Air quality dropped when I started cooking") and verify improvements (e.g., "The air purifier is working").

## What Changes

- **Data Logging**: Implement a background mechanism to record PM2.5 values every 10 seconds into a ring buffer.
- **UI Extension**: Add a "TREND" button to the Main Screen and a new "Chart" screen.
- **Chart Rendering**: Implement a line drawing algorithm that auto-scales to the data range and fits the screen width.
- **Integration Testing**: Add automated tests to verify the chart screen loads and renders correctly.
- **CI Reliability**: Fix BMP generation issues in the emulator to ensure reliable visual regression testing in CI environments.

## Capabilities

### New Capabilities

- `pm25-chart`: Visualizing historical PM2.5 data trends.

### Modified Capabilities

- `display-integration-test`: Enhanced to support 24-bit BMP comparisons and mouse input simulation for navigation.

## Impact

- **Code**: `App.cpp` logic extended with `APP_STATE_CHART` and `DrawChartScreen`. `LCD_Driver_SDL.cpp` updated for BMP correctness.
- **Resources**: Moderate RAM usage increase (100-400 bytes) for data buffer.
- **UX**: New navigation flow added.
