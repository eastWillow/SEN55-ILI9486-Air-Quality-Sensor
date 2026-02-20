## Context

The Air Quality Sensor current displays real-time PM2.5 data but lacks historical context. To help users understand air quality trends over time, we need a historical chart. Since memory on microcontrollers (like Arduino/ESP) is limited, we need an efficient way to store this data. A circular buffer/shift register updating every 10 seconds is chosen, and the number of stored points will be strictly limited to what can fit on the screen width to avoid scrolling complexity and memory overhead.

## Goals / Non-Goals

**Goals:**
- Provide a visual historical trend of PM2.5 readings.
- Ensure constant memory utilization by using a circular buffer approach (or simple shift array).
- Automatically scale the Y-axis to provide meaningful visual resolution based on the current data window.
- Make the new screen accessible from the main UI seamlessly.

**Non-Goals:**
- Panning or zooming the chart (data is strictly limited to screen width).
- Persisting historical data across reboots (volatile memory only).
- Tracking trends for sensors other than PM2.5 in this iteration.

## Decisions

- **Data Structure:** Use a fixed-size array (`std::array` or raw array) functioning as a shift register or circular buffer. A shift register (shifting elements left and appending to the right) is conceptually simpler for rendering a left-to-right chart, though a circular buffer is technically O(1) for updates. Given the low update rate (10s) and small size (e.g., ~300 pixels max), shifting is acceptable. Let's decide on a simple shift approach for simplicity mapping to X coordinates.
- **Update Frequency:** Data is sampled and stored every 10 seconds.
- **Y-Axis Auto-scaling:** The UI component will calculate the `min` and `max` of the current buffer before rendering to set the scale.
- **Y-Axis Label Clearing:** To prevent overlapping digits when the scale changes (e.g., transitioning from "100" to "20"), the specific area for the Y-axis labels MUST be cleared with the background color before drawing new strings.
- **Refactoring Static Variables:** Static variables in `App.cpp` will be moved/refactored into class members or anonymous namespaces where appropriate to fix forward declaration issues, keeping the global namespace clean and preventing initialization order fiascos.

## Risks / Trade-offs

- [Memory Constraints] → Pre-allocate the fixed-size array. The size should be derived directly from the display width allocated for the chart area.
- [Blocking rendering] → The drawing routine must be optimized. Drawing a line chart entails drawing N-1 line segments. The generic graphics library `drawLine` should be sufficient, but we must ensure it doesn't block the UI thread too long.
