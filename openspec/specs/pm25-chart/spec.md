# PM2.5 Trend Chart Capability

## Introduction
The PM2.5 Trend Chart provides users with a visual history of air quality data, allowing them to identify trends over time directly on the device. This complements the real-time numerical display by offering context to the current readings.

## Requirements

### User Interface
- **Access**: Accessible via a "TREND" button on the Main Screen.
- **Layout**:
  - Full-screen chart view.
  - "BACK" button to return to the Main Screen.
  - Y-Axis: Auto-scaling based on the maximum value in the visible history (minimum 10 ug/m3).
  - X-Axis: Represents time, filling the screen width.
- **Style**: Simple line chart (red line) on a white background with black axes.

### Data Handling
- **Metric**: Tracks PM2.5 concentration (ug/m3).
- **Interval**: Records a data point every 10 seconds.
- **Storage**: Retains historical data in a circular buffer (FIFO).
  - **PC/Emulator**: ~200 points (approx 33 minutes).
  - **Arduino**: ~50 points (approx 8 minutes) to conserve RAM.
- **Persistence**: Data clears on device reset (RAM-only storage).

### Interaction
- **Navigation**:
  - Main Screen -> Trend Chart (via "TREND" button).
  - Trend Chart -> Main Screen (via "BACK" button).
- **Updates**:
  - Chart updates automatically every 10 seconds while viewing.
  - Background data collection continues regardless of the active screen.

## Technical Constraints
- **Arduino Uno RAM**: Limited to 2KB. The data buffer uses `uint16_t` to minimize footprint (100-400 bytes).
- **Graphics**: Uses primitive drawing functions (`DrawLine`) compatible with the ILI9486 driver.
