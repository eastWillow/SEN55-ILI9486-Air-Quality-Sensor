## 1. Project Setup & Refactoring

- [x] 1.1 Refactor static variable declarations in `App.cpp` to avoid forward declaration issues

## 2. Core Logic (Trend Data)

- [x] 2.1 Implement a circular buffer / shift array for PM2.5 data sized to display width
- [x] 2.2 Add a 10-second periodic timer to record the current PM2.5 reading into the buffer

## 3. UI Implementation & Navigation

- [x] 3.1 Add a "Trend" button to the main screen UI
- [x] 3.2 Create the new Trend chart screen component
- [x] 3.3 Implement UI navigation logic to switch to the Trend screen and back to the main screen

## 4. Chart Rendering

- [x] 4.1 Implement Y-axis auto-scaling calculation (min/max of current data window)
- [x] 4.2 Implement the line drawing routine to render the historical data points on the Trend screen
- [x] 4.3 Fix Y-axis label overlapping by clearing label area before redraw

## 5. Testing & Verification

- [x] 5.1 Implement a new integration test `CheckpointChart` simulating the Trend button click
- [x] 5.2 Implement `CheckpointYAxisStability` integration test for scaling regression
- [x] 5.3 Update visual reference screenshots to reflect the UI changes

## 6. Verification

- [x] 6.1 Re-run `xvfb-run -a ctest -R "DebouncePerformanceTest" --output-on-failure --output-junit testReport-perf.xml`
- [x] 6.2 Re-run `xvfb-run -a ctest -R "DisplayIntegrationTest" --output-on-failure --output-junit testReport-smoke.xml`
- [x] 6.3 Re-run `xvfb-run -a ctest -R "CoreLibTest" --output-on-failure --output-junit testReport-core.xml`
- [x] 6.4 Re-run `xvfb-run -a ctest -R "GUITest" --output-on-failure --output-junit testReport-gui.xml`
- [x] 6.5 Perform final check on all GUI-related unit tests to ensure no regressions
