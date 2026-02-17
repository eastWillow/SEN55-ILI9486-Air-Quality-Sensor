## Why

The emulator smoke test (`emulator.yml`) validates that the DisplayEmulator can compile and run without crashing, but it does **not** verify rendering correctness. UI regressions — shifted text, broken color palettes, missing sensor readings — can slip through undetected. A visual regression testing layer is needed to catch these before they reach `main`.

The smoke test infrastructure is stable, specs are documented, and the development workflow is established — making this the right time to add the next tier of testing.

## What Changes

- **Refactor emulator for testability**: Extract the main loop into a controllable `EmulatorEngine` class with `stepFrame()` and `captureScreenshot()` APIs
- **Ensure deterministic rendering**: Replace `time()` calls with injectable/fixed clock, seed or eliminate animation randomness, verify fixed sensor mock data
- **Generate and commit reference screenshots**: Create known-good golden images for each checkpoint (startup, sensors displayed, final UI)
- **Implement GTest integration test suite**: Multi-checkpoint screenshot capture with pixel-level comparison using ImageMagick
- **Create CI workflow** (`integration-test.yml`): Independent visual regression pipeline with artifact upload on failure
- **Document reference update process**: How to update golden images when UI intentionally changes

## Capabilities

### New Capabilities

_(none — the `display-integration-test` capability spec already exists and covers the requirements)_

### Modified Capabilities

- `display-integration-test`: Implementing the requirements already specified. No spec changes needed — this change is about **building what's already been specified**.

## Impact

- **Code**: Emulator main loop refactored into `EmulatorEngine` class (new test hooks, existing behavior preserved)
- **Build**: `tests/CMakeLists.txt` updated to build integration test suite linking CoreLib
- **CI**: New `.github/workflows/integration-test.yml` workflow added
- **Dependencies**: ImageMagick added as CI dependency (already available via `apt-get`)
- **Repository**: Reference screenshots (~few hundred KB BMP files) committed to `tests/integration/reference_screenshots/`
- **README**: Integration test badge and documentation added
