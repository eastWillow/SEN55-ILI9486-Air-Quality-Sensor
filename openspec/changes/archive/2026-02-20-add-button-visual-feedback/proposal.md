# Proposal: Button Visual Feedback

Add visual feedback to interactive buttons (INFO and BACK) by inverting their colors for 100ms upon being pressed, before transitioning to the next screen.

## Problem

Currently, buttons in the application provide no visual feedback when pressed. This can lead to a poor user experience as the user is unsure if their touch was registered, especially if there is a slight delay in screen transition.

## Proposed Solution

Implement a short (100ms) visual feedback phase for all interactive buttons.
- When a button is pressed, its colors will be inverted (background becomes foreground color, text becomes background color).
- The application will maintain this inverted state for 100ms using the `TimeProvider`.
- After 100ms, the actual state transition (e.g., from Main to Info screen) will occur.

## What Changes

- `App.cpp`: Add logic to handle button feedback duration.
- `App.cpp`: Add a helper function to draw inverted buttons.
- `App.cpp`: Update `App_Loop` to handle the feedback timing and delay transitions.
- `display_integration_test.cpp`: Add a test case to verify the inverted button state is visible at the correct time.
- Update integration golden samples (reference screenshots) if necessary, or add new ones for the feedback state.

## Capabilities

### New Capabilities
- `button-visual-feedback`: Provide immediate visual feedback for touch interactions on buttons.

### Modified Capabilities
- `ui-navigation`: Update navigation logic to include the feedback delay.

## Impact

- Improved UX with tactile-like visual feedback.
- Slight increase in complexity in the application loop to manage timing.
- Integration tests will now need to account for this intermediate state.
