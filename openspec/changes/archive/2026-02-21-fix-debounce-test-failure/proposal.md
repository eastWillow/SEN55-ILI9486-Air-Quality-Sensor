## Why

The introduction of the 100ms button visual feedback phase in the `add-button-visual-feedback` change broke the `DebouncePerformanceTest`. The test suite was designed for immediate state transitions and did not account for the deferred transition logic now required for visual feedback.

## What Changes

Update the `DebouncePerformanceTest` suite to align with the new application behavior. This involves:
- Advancing time to account for the 100ms feedback phase.
- Verifying that state transitions are correctly deferred during feedback.
- Updating the clock rollover test to handle the additional time steps required for feedback completion.

## Capabilities

### Modified Capabilities
- `button-visual-feedback`: The feedback phase requirement is technically correct, but the testing strategy for performance must be updated to match the implementation.
- `ui-navigation`: The deferred transition requirement needs to be verified by performance tests.

## Impact

- `tests/unit/debounce_performance_test.cpp`: All test cases will be updated to reflect the new timing and deferred state transitions.
