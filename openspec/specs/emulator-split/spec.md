# Capability: emulator-split

## Overview
The DisplayEmulator provides two execution modes to support both automated CI testing (smoke tests) and interactive development. This capability focuses on **execution validation** - ensuring the emulator can run without crashing in headless CI environments.

**Note**: This is distinct from `display-integration-test`, which performs visual regression testing via screenshot comparison.

## ADDED Requirements

### Requirement: Dual Execution Modes
The emulator must support both automated test mode and interactive development mode.

#### Scenario: Automated Test Mode (Smoke Test)
- **WHEN** the emulator is launched with the `--test` argument
- **THEN** it runs for a fixed number of frames (25)
- **AND** it saves the final screen content to `screenshot.bmp`
- **AND** it terminates automatically with exit code 0 (success)
- **AND** no manual interaction is required

**Purpose**: Validate that the emulator can execute in headless CI environments (xvfb-run) without crashes.

#### Scenario: Interactive Development Mode
- **WHEN** the emulator is launched without arguments
- **THEN** it runs indefinitely until manually closed
- **AND** it accepts and processes mouse/keyboard input for interaction
- **AND** the window remains responsive to user actions

**Purpose**: Enable developers to manually test UI rendering and interactions locally.

---

### Requirement: CI Smoke Test Workflow
The emulator must have an automated CI workflow that validates basic execution.

#### Scenario: Emulator Smoke Test in CI
- **GIVEN** a GitHub Actions workflow (`emulator.yml`)
- **WHEN** code is pushed to main or a PR is opened
- **THEN** the workflow builds the emulator
- **AND** runs it with `xvfb-run ./DisplayEmulator --test`
- **AND** the test passes if exit code is 0
- **AND** on failure, uploads `screenshot.bmp` as an artifact for debugging

**Validation Criteria**:
- ✅ Emulator compiles successfully
- ✅ SDL2 initializes in virtual framebuffer
- ✅ Runs without segfaults or crashes
- ✅ Generates screenshot file
- ❌ Does NOT validate screenshot content (see `display-integration-test`)

---

### Requirement: Screenshot Artifact Capture
Smoke tests must capture and preserve screenshots for inspection.

#### Scenario: Screenshot Artifact Upload
- **GIVEN** the emulator completes execution (success or failure)
- **WHEN** the CI workflow completes
- **THEN** `screenshot.bmp` is uploaded as a GitHub Actions artifact named `emulator-screenshot`
- **AND** developers can download it to:
  - Verify the emulator rendered something
  - Inspect output for obvious rendering issues
  - Use as reference when creating integration tests

**Note**: This artifact is for **inspection only**, not automated validation. Visual regression testing will be handled by the `display-integration-test` capability.

---

### Requirement: Distinction from Component Tests
The smoke test workflow MUST be clearly separated from component/unit test workflows.

#### Scenario: Test Layer Responsibilities
- **WHEN** Evaluating test coverage
- **THEN** Smoke tests (`emulator.yml`) validate binary integration
- **AND** Component tests (`tests.yml`) validate `CoreLib` correctness
- **AND** Both test types serve distinct purposes without duplication

**Test Comparison**:

| Test Type | Smoke Test (`emulator.yml`) | Component/Unit Tests (`tests.yml`) |
|-----------|--------------------------|----------------------------------|
| **Target** | `DisplayEmulator` binary | `CoreLib` components |
| **Method** | Execute binary | Link library & call functions |
| **Purpose** | "Can it run?" | "Does it work correctly?" |
| **Scope** | Full integration | Isolated components |
| **xvfb-run needed?** | ✅ Yes (runs SDL2 app) | ✅ Yes (`CoreLib` uses SDL2) |

#### Scenario: Both Need xvfb-run
- **WHEN** Running smoke tests
- **THEN** `xvfb-run` is required because `DisplayEmulator` calls `SDL_Init()`
- **WHEN** Running component tests
- **THEN** `xvfb-run` is required because `CoreLib` contains `LCD_Driver_SDL.cpp`

**Rationale**: Both test types use SDL2, but at different abstraction layers - smoke test exercises the full application, component tests validate individual SDL function calls.

### Requirement: Test Coverage Documentation
Documentation MUST clarify what each test type validates.

#### Scenario: Smoke Test Validation
- **WHEN** `emulator.yml` executes
- **THEN** It validates: Binary builds, SDL initializes, No crashes, Exit code 0
- **AND** It does NOT validate: Component behavior, Business logic correctness

#### Scenario: Component Test Validation  
- **WHEN** `tests.yml` executes
- **THEN** It validates: App initialization, Sensor data processing, Display functions, Error handling
- **AND** It does NOT validate: Final binary integration (that's smoke test)

**Testing Pyramid**:
```
    △ emulator.yml (Smoke - Binary Integration)
   △△ Visual regression (Future - Screenshot comparison)
  △△△ tests.yml (Component - CoreLib validation)
```

---

## Distinction from Integration Tests

| Capability | emulator-split (Smoke Test) | display-integration-test |
|------------|----------------------------|--------------------------|
| **Purpose** | Can it run? | Does it render correctly? |
| **Validation** | Exit code 0 | Pixel-level screenshot comparison |
| **Screenshot Use** | Debugging artifact only | Regression test assertion |
| **CI Workflow** | `emulator.yml` | `integration-test.yml` (future) |
| **Speed** | Fast (~5s) | Slower (~30s+) |
| **Dependencies** | Xvfb, SDL2 | + ImageMagick, GTest, reference images |

---

## CI Workflow Details

### File: `.github/workflows/emulator.yml`
**Name**: Display Emulator CI  
**Triggers**: Push to main, Pull requests  
**Job**: `build-and-run`

**Steps**:
1. Checkout code with submodules
2. Install dependencies: `xvfb`, `libsdl2-dev`, `libsdl2-mixer-dev`, `libgtest-dev`
3. Configure CMake: `cmake ..`
4. Build: `cmake --build .`
5. Run emulator test: `xvfb-run ./DisplayEmulator --test`
6. Upload screenshot artifact (always, with name `emulator-screenshot`)

**Success Criteria**: Exit code 0 from step 5

**Artifacts**: `screenshot.bmp` uploaded for every run (inspection only, not validated)

---

## Local Development Workflow

### Building and Running Locally

```bash
mkdir build
cd build
cmake ..
cmake --build .
./DisplayEmulator          # Interactive mode
./DisplayEmulator --test   # Test mode (25 frames + screenshot)
```

### Testing CI Workflows Locally

Using [GitHub Act](https://github.com/nektos/act):

```bash
gh extension install https://github.com/nektos/gh-act
gh act push  # Simulates push event locally
```

This allows developers to test CI workflows without pushing to GitHub.

