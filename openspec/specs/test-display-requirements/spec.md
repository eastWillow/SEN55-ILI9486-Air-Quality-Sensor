# Capability: test-display-requirements

## Overview
Requirements for running SDL2-based tests in headless CI environments. This capability defines how graphics-dependent tests must be executed when no physical display is available.

## Requirements

### Requirement: Virtual Display Provision
CI workflows SHALL provide a virtual framebuffer for tests that require graphical display access.

#### Scenario: SDL2 Test Execution in Headless CI
- **WHEN** Tests use SDL2, OpenGL, or other display-dependent libraries
- **THEN** The CI workflow MUST invoke the test runner under `xvfb-run`
- **AND** SDL2_Init() completes successfully without display errors
- **AND** The virtual display is available to all child processes

#### Scenario: Process Inheritance
- **WHEN** A test spawns child processes that use graphics
- **THEN** The child processes MUST inherit the DISPLAY environment variable
- **AND** Child processes can initialize graphics without additional xvfb-run wrappers

### Requirement: Selective Application
Virtual display SHALL only be used when necessary to avoid unnecessary overhead.

#### Scenario: Graphics Tests
- **WHEN** Tests link SDL2 libraries or spawn SDL2 executables
- **THEN** The test command MUST use xvfb-run wrapper
- **AND** Tests execute successfully in headless environment

#### Scenario: Non-Graphics Tests
- **WHEN** Tests do not use graphics libraries (pure logic, compile-only checks)
- **THEN** The test command MUST NOT use xvfb-run
- **AND** Tests execute directly without virtual display overhead

**Examples**:
- ✅ Needs xvfb-run: `xvfb-run ctest` (tests link SDL2)
- ✅ Needs xvfb-run: `xvfb-run ./DisplayEmulator --test` (SDL2 app)
- ❌ Doesn't need: `arduino-cli compile` (compilation only)
- ❌ Doesn't need: `npm run build` (build artifacts)

### Requirement: Mental Model Documentation
The relationship between xvfb-run and graphics libraries SHALL be clearly documented.

#### Scenario: Developer Understanding
- **WHEN** Developers add new SDL2-based tests
- **THEN** Documentation MUST explain that SDL2 requires display
- **AND** CI workflow examples MUST show xvfb-run usage
- **AND** The rule "SDL = GUI = xvfb-run" is clearly stated

**Mental Model**:
```
SDL2 dependency  →  Graphics rendering  →  xvfb-run required in CI
OpenGL           →  GPU rendering       →  xvfb-run required in CI
Headless CI      →  No physical display →  Virtual framebuffer needed
```
