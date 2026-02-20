# Design: Button Visual Feedback

This design introduces a visual feedback mechanism for button presses in the Air Quality Sensor application.

## User Interface

### General Feedback Behavior
- Every interactive button (initially `INFO` and `BACK`) will provide visual feedback upon being pressed.
- The feedback consists of inverting the button's colors:
  - Background (white) becomes the foreground color (blue).
  - Text/Border (blue) becomes the background color (white).
- The inverted state persists for a fixed duration of **100ms**.
- The actual action (e.g., screen transition) is deferred until the 100ms feedback period has elapsed.

## Technical Design

### State Management
The application loop will manage the feedback state using the following variables:
- `bool inFeedback`: Flag indicating if the application is currently showing button feedback.
- `unsigned long feedbackStartTime`: The timestamp when the feedback started.
- `AppState pendingState`: The state to transition to after feedback ends.
- `int pressedButtonId`: Identifier for the button that was pressed.

### Components

#### Button Helper
A new helper function `DrawButton` will be introduced to standardize button drawing and support inversion:
```cpp
void DrawButton(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const char* label, bool inverted);
```

#### Application Loop Update
`App_Loop` will be modified to:
1. Detect touch events.
2. If a button is pressed and not already in feedback:
   - Set `inFeedback = true`.
   - Record `feedbackStartTime` from `appTime->getMillis()`.
   - Set `pendingState` and `pressedButtonId`.
   - Immediately draw the button in inverted mode.
3. If `inFeedback` is true:
   - Check if `currentMillis - feedbackStartTime >= 100`.
   - If true:
     - Set `inFeedback = false`.
     - Update `currentState = pendingState`.
     - Perform the screen transition (redraw the entire screen).
   - If false:
     - Keep the current screen (waiting for feedback to expire).

### Verification Plan

#### Automated Tests
- **Integration Test**: Add a new test case in `display_integration_test.cpp`.
  - Simulate a touch on the `INFO` button.
  - Advance time by 50ms (less than 100ms).
  - Capture a screenshot and verify it shows the inverted `INFO` button on the Main screen.
  - Advance time by another 60ms (total > 100ms).
  - Capture a screenshot and verify it shows the Info screen.

#### Manual Verification
- Deploy to device/emulator and verify that buttons flash blue when tapped.
- Verify that the transition feels responsive but shows the flash clearly.
