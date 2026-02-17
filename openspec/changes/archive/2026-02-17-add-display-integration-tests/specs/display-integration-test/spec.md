## ADDED Requirements

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
