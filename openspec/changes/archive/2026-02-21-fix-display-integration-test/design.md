## Context

The `DisplayIntegrationTest` helps ensure visual consistency of the application's LCD output. Recent changes introduced a 100ms visual feedback window for button presses and fixed Y-axis label overlaps on the trend chart. These changes have invalidated the existing baseline screenshots used for comparison in `CheckpointButtonFeedback` and `CheckpointChart`.

## Goals / Non-Goals

**Goals:**
- Update integration test baselines to reflect intentional UI enhancements.
- Verify that `CheckpointButtonFeedback` correctly captures the feedback state.
- Ensure all integration tests pass with the new visual standards.

**Non-Goals:**
- Changing the underlying UI logic (this change is strictly about aligning tests with the existing logic).

## Decisions

- **Update Reference Screenshots**: Since the UI changes (button feedback and Y-axis fix) are intentional and verified in other contexts, we will update `tests/integration/reference_screenshots/` with the current "actual" outputs from the emulator.
- **Test Synchronization**: Ensure `MockTimeProvider` is used consistently to capture precisely the state intended (e.g., 50ms into a 100ms feedback window).

## Risks / Trade-offs

- **Baseline Fragility**: Visual integration tests are sensitive to font rendering, coordinates, and color changes. Updating them requires manual verification that the "actual" image is indeed correct.
- **Risk**: Blindly updating screenshots could mask an actual regression. Manual inspection of `diff_*.bmp` files produced during local runs is required.
