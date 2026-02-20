## Why
Users need the ability to visualize how PM2.5 air quality measurements have changed over time. A historical trend chart provides this context to understand if air quality is improving or worsening, and allows users to spot patterns and trends at a glance.

## What Changes
- Implement a circular buffer/shift register to store historical PM2.5 data, updating every 10 seconds.
- Restrict stored data to only fit the screen size, preventing the need for horizontal scrolling.
- Add a new "Trend" button to the main screen UI.
- Create a new trend chart screen featuring a simple line graph with an auto-scaling Y-axis.
- Fix Y-axis label overlapping by ensuring the label area is cleared before redrawing when the scale changes.
- Adopt a line chart visual style similar to matplotlib.
- Add a new integration test `CheckpointChart` to simulate clicking the Trend button and waiting for data to populate.
- Add a new regression test `CheckpointYAxisStability` to ensure Y-axis labels don't leak pixels when scaling up and down.
- Update reference screenshots to reflect the new UI changes.
- Refactor static variable declarations in `App.cpp` to avoid forward declaration issues.

## Capabilities

### New Capabilities
- `pm25-trend-chart`: Core logic for data buffering, UI rendering for the line graph, and Y-axis auto-scaling calculation.

### Modified Capabilities
- `ui-navigation`: Addition of the Trend button to the main screen and navigating to the trend chart screen.

## Impact
- **UI:** New trend button on the main screen, and a newly implemented trend chart screen UI.
- **State Management:** Introduces a historical snapshot array/buffer that updates periodically.
- **Testing Context:** Extends integration tests with a new visual checkpoint (`CheckpointChart`) and visual baseline definitions.
- **Codebase Health:** Cleaned up static declarations in `App.cpp`.
