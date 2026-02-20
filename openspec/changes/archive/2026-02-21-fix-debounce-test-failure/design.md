## Technical Architecture

The fix focuses on updating the test logic in `debounce_performance_test.cpp` to correctly simulate the application's timing lifecycle:
1. **Touch Event**: Initiates the feedback phase.
2. **Feedback Phase (100ms)**: State transition is deferred; button colors are inverted.
3. **Transition**: Occurs after 100ms has elapsed.

The `MockTimeProvider` will be used to precisely advance time and verify these stages.

## Data Structures

No changes to application data structures.

## Implementation Details

### `NonBlockingDebounce` Test
- Advance time by 50ms after the first click.
- Verify state is still `APP_STATE_MAIN` (deferred).
- Advance another 60ms (total > 100ms).
- Call `App_Loop` and verify state is `APP_STATE_INFO`.
- Increase the second click's wait time to 250ms to ensure both debounce (200ms) and feedback (100ms) logic are properly handled.

### `ClockRollover` Test
- Adjust time advancements to 150ms per transition to clear the 100ms feedback phase.
- Ensure total elapsed time between interactions exceeds the 200ms debounce window plus the 100ms feedback window.

## Verification Plan

### Automated Tests
- Run `ctest -R DebouncePerformanceTest` to verify the fix.
- Ensure `testReport-perf.xml` is generated and shows all tests passing.

### Manual Verification
- Not required as this is a test-only fix, but the emulator can be used to confirm the 100ms "feel" remains consistent.
