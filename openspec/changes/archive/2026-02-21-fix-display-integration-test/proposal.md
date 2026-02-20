## Why

The `DisplayIntegrationTest` is failing on `CheckpointButtonFeedback` and `CheckpointChart` due to visual mismatches between actual rendering and reference screenshots. This likely stems from recent UI changes, specifically the introduction of the 100ms visual feedback phase for button presses, which affects the rendered state captured in integration tests.

## What Changes

- Resolve visual regressions in `DisplayIntegrationTest`.
- Update `tests/integration/display_integration_test.cpp` if timing or coordinate adjustments are needed.
- Update/regenerate reference screenshots in `tests/integration/reference_screenshots/` to reflect intended UI behavior.

## Capabilities

### Modified Capabilities
- `testing-infrastructure`: Update integration test checkpoints to align with the new visual feedback mechanism and ensure stable verification.

## Impact

- `tests/integration/display_integration_test.cpp`
- `tests/integration/reference_screenshots/feedback_info.bmp`
- `tests/integration/reference_screenshots/trend_chart.bmp`
