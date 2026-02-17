# Capability: display-integration-test

## Overview
Visual regression testing for the DisplayEmulator through automated screenshot comparison. This capability validates UI rendering correctness by comparing emulator output against known-good reference images.

**Context**: This capability is separate from the `emulator-split` smoke test. The smoke test (`emulator.yml`) validates execution only (can it run without crashing?). Visual regression testing (screenshot comparison) was intentionally removed from the smoke test suite and will be implemented here when reference images are available.

## ADDED Requirements


### Requirement: Checkpoint-Based Screenshot Capture
The emulator must support programmatic control to capture screenshots at specific rendering checkpoints.

#### Scenario: Multi-Checkpoint Test Execution
- **GIVEN** the emulator is running in integration test mode
- **WHEN** controlled via GTest/GMock test framework
- **THEN** the test can trigger screenshot capture at predefined checkpoints
- **AND** each checkpoint generates a uniquely named BMP file
- **AND** checkpoints capture deterministic rendering states

**Example Checkpoints**:
- Frame 0: Initial startup state
- Frame 10: Sensor data first displayed
- Frame 25: Full UI rendered with animations settled

---

### Requirement: Deterministic Rendering
The emulator must produce pixel-perfect identical output when run with the same inputs.

#### Scenario: Reproducible Test Runs
- **GIVEN** the emulator is in test mode
- **WHEN** the test executes multiple times
- **THEN** sensor data is fixed (seeded/mocked)
- **AND** timestamps are fixed (not using wall clock)
- **AND** animation states are deterministic (fixed seed/no randomness)
- **AND** generated screenshots are byte-identical across runs

---

### Requirement: Reference Image Storage
Reference screenshots must be version-controlled alongside the codebase.

#### Scenario: Reference Image Management
- **GIVEN** a successful test run producing correct output
- **WHEN** a developer validates the visual output
- **THEN** the screenshot can be committed to `tests/integration/reference_screenshots/`
- **AND** reference images are tracked in Git
- **AND** reference images have descriptive names (e.g., `checkpoint_startup.bmp`, `checkpoint_sensors.bmp`)

---

### Requirement: Visual Comparison with Tolerance
Screenshot comparison must detect visual regressions while allowing for acceptable variations.

#### Scenario: Pixel Difference Analysis
- **GIVEN** a reference screenshot and current test output
- **WHEN** the CI workflow compares images using ImageMagick
- **THEN** pixel differences are counted using `compare -metric AE`
- **AND** a threshold (e.g., 100 pixels) is defined for acceptable variation
- **AND** tests fail if differences exceed the threshold
- **AND** a diff image is generated highlighting changed pixels

---

### Requirement: CI Artifact Upload on Failure
When visual regression is detected, debugging artifacts must be preserved.

#### Scenario: Test Failure Debugging
- **GIVEN** a visual regression test failure
- **WHEN** the CI workflow detects pixel differences exceeding threshold
- **THEN** the following artifacts are uploaded:
  - Current screenshot (`screenshot_checkpoint_X.bmp`)
  - Reference screenshot (from `tests/integration/reference_screenshots/`)
  - Diff image showing pixel-level differences
- **AND** developers can download artifacts to analyze the regression

---

### Requirement: Independent CI Workflow
Integration tests run independently from smoke tests for parallel execution.

#### Scenario: CI Pipeline Execution
- **GIVEN** a pull request or push to main
- **WHEN** GitHub Actions triggers CI workflows
- **THEN** the integration test workflow runs independently
- **AND** it does not depend on `emulator.yml` smoke test completion
- **AND** both workflows can execute in parallel

---

## Dependencies

### Build Dependencies
- **CMake**: Build system
- **SDL2**: Graphics backend for emulator
- **Xvfb**: Virtual framebuffer for headless CI execution
- **ImageMagick**: Image comparison tool (`compare` command)
- **GTest/GMock**: Test framework for programmatic emulator control

### Test Data Dependencies
- Reference screenshots in `tests/integration/reference_screenshots/`
- Fixed test data for sensors, timestamps, and animations

---

## Architecture Notes

### Test Execution Flow
```
┌─────────────────────────────────────────────────────┐
│ GTest Integration Test                              │
├─────────────────────────────────────────────────────┤
│                                                     │
│  1. Initialize Emulator (deterministic mode)       │
│  2. Run to Checkpoint 1 → Capture screenshot_1.bmp │
│  3. Run to Checkpoint 2 → Capture screenshot_2.bmp │
│  4. Run to Checkpoint N → Capture screenshot_N.bmp │
│  5. Shutdown Emulator                               │
│                                                     │
│  6. For each checkpoint:                            │
│     - Load reference image                          │
│     - Compare with ImageMagick                      │
│     - Assert pixel difference < threshold           │
│     - Generate diff on mismatch                     │
│                                                     │
└─────────────────────────────────────────────────────┘
```

### Distinction from Smoke Test
| Aspect | Smoke Test (`emulator.yml`) | Integration Test |
|--------|----------------------------|------------------|
| **Purpose** | Can it run? | Does it render correctly? |
| **Execution** | Single run to completion | Checkpoint-controlled |
| **Output** | Single final screenshot | Multiple checkpoint screenshots |
| **Validation** | Exit code 0 (no crash) | Pixel-level comparison |
| **Duration** | Fast (~5 seconds) | Slower (~30 seconds) |
| **Dependencies** | Xvfb, SDL2 | + ImageMagick, GTest, reference images |

---

## Future Enhancements (Out of Scope)

- **Adaptive Thresholds**: Per-checkpoint tolerance levels for areas with known variability
- **Visual Diff Reports**: HTML report with side-by-side comparisons
- **Baseline Update Automation**: PR workflow to auto-update references on approval
- **Cross-Platform Testing**: Run on macOS/Windows runners to catch platform-specific rendering bugs

## Implemented Requirements (from add-display-integration-tests)

### Requirement: EmulatorEngine Test API
The emulator SHALL expose a programmatic API for test-controlled execution, enabling GTest to drive frame stepping and screenshot capture without spawning the binary.

#### Scenario: GTest-Controlled Multi-Checkpoint Execution
- **WHEN** a GTest integration test instantiates `EmulatorEngine`
- **THEN** the test can call `initialize()` to set up the emulator with a `SensorIntf*`
- **AND** the test can call `stepFrames(n)` to advance rendering by `n` frames
- **AND** the test can call `captureScreenshot(path)` to save the current frame
- **AND** the test can call `shutdown()` to clean up resources
- **AND** the `EmulatorEngine` class is part of CoreLib (linkable by tests)

#### Scenario: Startup Checkpoint Validation
- **WHEN** the test captures a screenshot at frame 0 (after initialization)
- **THEN** the screenshot matches the `startup.bmp` reference within the pixel threshold
- **AND** differences exceeding the threshold cause test failure with diff image

#### Scenario: Final Checkpoint Validation
- **WHEN** the test captures a screenshot at frame 25 (full UI rendered)
- **THEN** the screenshot matches the `final.bmp` reference within the pixel threshold
- **AND** differences exceeding the threshold cause test failure with diff image

---

### Requirement: Reference Image Update Process
Developers SHALL have a documented process for updating reference images when the UI intentionally changes.

#### Scenario: Updating References After UI Change
- **WHEN** a developer intentionally changes the UI rendering
- **THEN** they run the emulator in test mode locally to generate new screenshots
- **AND** they visually inspect the new screenshots for correctness
- **AND** they copy validated screenshots to `tests/integration/reference_screenshots/`
- **AND** they commit with a message explaining the UI change
- **AND** CI passes with the updated reference images

#### Scenario: Reference Image Documentation
- **WHEN** a developer looks at `tests/integration/reference_screenshots/`
- **THEN** a `README.md` file explains the update process
- **AND** each reference file has a descriptive name matching its checkpoint
