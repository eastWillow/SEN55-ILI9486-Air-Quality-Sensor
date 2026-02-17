# Software Architecture

## Overview
A layered architecture designed for portability across embedded hardware (Arduino) and desktop/web simulation (SDL2/WASM). The system is built around a core application logic that interacts with hardware through abstract interfaces.

## Component Diagram

```mermaid
graph TD
    App[Application Logic] --> SensorIntf[Sensor Interface]
    App --> Display[Display Driver]
    
    subgraph "Hardware Abstraction Layer"
        SensorIntf --> SensorReal[SensorReal (I2C)]
        SensorIntf --> SensorMock[SensorMock (Simulated)]
        Display --> LCD_Driver[LCD Driver (SPI/Parallel)]
        Display --> LCD_SDL[LCD SDL (Simulated)]
    end
    
    subgraph "Platform Implementations"
        SensorReal --> Wire[Arduino Wire Lib]
        LCD_Driver --> SPI[Arduino SPI Lib]
        LCD_SDL --> SDL2[SDL2 Library (PC)]
        LCD_SDL --> Emscripten[Emscripten (Web)]
    end
```


## Module Descriptions

### 1. Application Layer (`src/App`)
- **Responsibility**: Contains the main application loop, state machine, and high-level logic.
- **Key Files**:
    - `App.cpp`: Implementation of `App_Setup` and `App_Loop`.
    - `App.h`: Public API for the application layer.
- **Dependencies**: `SensorIntf` for data access, `Display` headers for UI rendering.

### 2. Sensor Abstraction (`src/Sensor`)
- **Responsibility**: Provides a unified interface for reading environmental data, abstracting the specific hardware implementation.
- **Interface**: `SensorIntf.h` defines the pure virtual interface.
- **Implementations**:
    - `SensorReal`: Communicates with physical SEN55 sensor via I2C (Arduino).
    - `SensorMock`: Generates simulated data patterns for testing without hardware (PC/CI).

### 3. Display Subsystem (`src/Display`)
- **Responsibility**: Handles all graphics rendering, text display, and touch input processing.
- **Modules**:
    - `LCD_Driver`/`LCD_Driver_SDL`: Low-level pixel pushing implementations for hardware vs. simulation.
    - `LCD_GUI`: Drawing primitives (Lines, Rectangles, Circles, Text).
    - `LCD_Touch`: Processing raw touch input and coordinate calibration.
    - `DEV_Config`: Hardware pin definitions and low-level IO macros.

## Build Systems & CI/CD
- **Arduino**: Built using `arduino-cli` targeting `arduino:avr:uno`. Verified in CI/CD pipeline.
- **PC Simulation**: Built using CMake with `g++` and SDL2 backend.
- **WebAssembly**: Built using Emscripten (emsdk) targeting generic WASM/HTML5 output.

## Testing Strategy

The project employs a multi-tier testing approach to ensure correctness across different validation levels.

### Testing Pyramid

```
                      ┌────────────────────────┐
                      │  Integration Tests     │ ← Visual Regression
                      │  (integration-test.yml)│   Multi-checkpoint
                      ├────────────────────────┤   GTest-controlled
                      │  Emulator Smoke Test   │ ← Execution Validation
                      │  (emulator.yml)        │   Crash detection
                      ├────────────────────────┤
                      │  Unit Tests            │ ← Component Isolation
                      │  (tests.yml)           │   Fast, deterministic
                      └────────────────────────┘
```

### Test Categories

#### 1. Unit Tests (`tests.yml`)
- **Purpose**: Validate individual components in isolation
- **Scope**: Sensor mocking, display primitives, data structures
- **Execution**: Standalone CMake build from `tests/` directory
- **Speed**: Fast (~10 seconds)
- **CI Workflow**: `.github/workflows/tests.yml`

#### 2. Emulator Smoke Test (`emulator.yml`)
- **Purpose**: Verify emulator can execute without crashing
- **Scope**: SDL2 initialization, basic rendering loop, screenshot generation
- **Validation**: Exit code 0 (no crash detection)
- **Speed**: Fast (~5 seconds)
- **CI Workflow**: `.github/workflows/emulator.yml`
- **Spec**: `openspec/specs/emulator-split/spec.md`

#### 3. Display Integration Tests
- **Purpose**: Visual regression testing via screenshot comparison
- **Scope**: UI rendering correctness, graphical output validation
- **Validation**: Pixel-level comparison against reference images
- **Control**: GTest/GMock for checkpoint-based execution
- **Speed**: Slower (~30+ seconds)
- **CI Workflow**: `.github/workflows/integration-test.yml`
- **Spec**: `openspec/specs/display-integration-test/spec.md`

#### 4. Platform Verification
- **Arduino CI** (`arduino.yml`): Compilation verification for embedded target
- **WASM CI** (`wasm.yml`): Web build + deployment to GitHub Pages

### Key Testing Principles

- **Deterministic Execution**: All automated tests use fixed seeds, timestamps, and sensor data
- **Independent Workflows**: Tests run in parallel for faster feedback
- **Artifact Preservation**: Failed tests upload debugging artifacts (screenshots, diffs)
- **Separation of Concerns**: Fast smoke tests catch crashes; slower integration tests catch regressions

