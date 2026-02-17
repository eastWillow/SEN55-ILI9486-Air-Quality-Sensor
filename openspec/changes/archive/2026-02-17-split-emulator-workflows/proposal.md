# Proposal: split-emulator-workflows

## Why

The current `main_pc.cpp` runs a fixed 25-frame loop (approx 1.25s), takes a screenshot, and exits. This supports a primitive form of "Workflow A" (automated test) but prevents "Workflow B" (interactive user exploring), which needs to run indefinitely.

## What Changes

*   **Refactor `main_pc.cpp`**: Decouple the loop logic from `main()` to support both finite (test) and infinite (interactive) runs.
*   **CLI Args**: Add command-line argument parsing to toggle between modes (`--test` for fixed loop, default for interactive).
*   **GTest Harness**: Implement a Google Test controller that runs the emulator in test mode and compares the output BMP against a reference.
*   **CI/CD**: Identify and update GitHub Actions workflows to use this new test rig.

## Capabilities

### New Capabilities

- `emulator-split`: Enables the emulator to run in two distinct modes: a headless/controlled test mode for CI/CD, and an interactive open-ended mode for users.

### Modified Capabilities

(None)

## Impact

*   `main_pc.cpp`: Logic flow changes.
*   `tests/`: New integration test file and reference image.
*   `.github/workflows/`: configuration updates.
