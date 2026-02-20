# Tasks: Button Visual Feedback

## 1. UI Infrastructure

- [x] 1.1 Extract button drawing logic into a helper function `DrawButton` in `App.cpp`
- [x] 1.2 Update `DrawMainScreen` and `DrawInfoScreen` to use `DrawButton`
- [x] 1.3 Verify that existing buttons still render correctly (fixed to match original golden samples exactly)

## 2. Feedback Implementation

- [x] 2.1 Add feedback state variables to `App.cpp` (`inFeedback`, `feedbackStartTime`, etc.)
- [x] 2.2 Modify `App_Loop` to handle touch detection with feedback initiation
- [x] 2.3 Implement the feedback timer check in `App_Loop` to execute deferred transitions
- [x] 2.4 Verify visual feedback manually in the emulator

## 3. Integration Testing

- [x] 3.1 Update `EmulatorEngine` or `DisplayIntegrationTest` if necessary (already should be possible via `TimeProvider`)
- [x] 3.2 Add `CheckpointButtonFeedback` test case to `display_integration_test.cpp`
- [x] 3.3 Run tests and verify screenshots show inverted buttons for the 100ms window
- [x] 3.4 Update golden samples (ONLY for the new feedback state; reverted startup/final to original)

## 4. Final Verification

- [x] 4.1 Run full suite of automated tests (`ctest`)
- [x] 4.2 Perform final manual check of the 100ms timing feel
