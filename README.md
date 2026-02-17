# SEN55-ILI9486 Air Quality Sensor

[![Arduino CI](https://github.com/eastwillowlearninglog/SEN55-ILI9486-Air-Quality-Sensor/actions/workflows/arduino.yml/badge.svg)](https://github.com/eastwillowlearninglog/SEN55-ILI9486-Air-Quality-Sensor/actions/workflows/arduino.yml)
[![Unit Tests](https://github.com/eastwillowlearninglog/SEN55-ILI9486-Air-Quality-Sensor/actions/workflows/tests.yml/badge.svg)](https://github.com/eastwillowlearninglog/SEN55-ILI9486-Air-Quality-Sensor/actions/workflows/tests.yml)
[![Emulator CI](https://github.com/eastwillowlearninglog/SEN55-ILI9486-Air-Quality-Sensor/actions/workflows/emulator.yml/badge.svg)](https://github.com/eastwillowlearninglog/SEN55-ILI9486-Air-Quality-Sensor/actions/workflows/emulator.yml)
[![WASM Build](https://github.com/eastwillowlearninglog/SEN55-ILI9486-Air-Quality-Sensor/actions/workflows/wasm.yml/badge.svg)](https://github.com/eastwillowlearninglog/SEN55-ILI9486-Air-Quality-Sensor/actions/workflows/wasm.yml)

Local Run Command :

```bash
mkdir build
cd build
cmake ..
cmake --build .
./DisplayEmulator
```

Local Run Test Command :

```bash
gh extension install https://github.com/nektos/gh-act
gh act push
```

Web mockup Display:
https://eastwillowlearninglog.github.io/SEN55-ILI9486-Air-Quality-Sensor/

Software Bill of Materials:
* ILI9486 Driver Form [3.5inch TFT Touch Shield](https://www.waveshare.com/wiki/3.5inch_TFT_Touch_Shield)
* SEN55 Library From [arduino-i2c-sen5x](https://github.com/Sensirion/arduino-i2c-sen5x)
* SEN55 Library Depency From [arduino-core](https://github.com/Sensirion/arduino-core)

Acknowledging AI Usage:
* Code assistance and debugging for the LCD driver and SEN55 implementation provided by Google's Gemini.

---

## Emulator Testing

The DisplayEmulator supports multiple testing modes to validate both execution and rendering correctness.

### 1. Interactive Mode (Local Development)
Run the emulator without any arguments to start an interactive session with mouse/keyboard input.

```bash
./build/DisplayEmulator
```

**Use Case**: Manual UI testing, visual inspection, interaction debugging.

---

### 2. Test Mode (CI Smoke Test)
Run the emulator with the `--test` flag for automated execution validation.

```bash
./build/DisplayEmulator --test
```

**Behavior**:
- Runs for exactly 25 frames (deterministic duration)
- Captures final screen to `screenshot.bmp`
- Exits automatically with code 0 on success
- Validated in CI via `.github/workflows/emulator.yml`

**Purpose**: Smoke test ensuring the emulator can execute in headless CI environments (xvfb-run) without crashes. This validates SDL2 initialization, basic rendering loops, and screenshot generation. **Does not validate rendering correctness** - see Integration Tests below.

---

### 3. Integration Testing (Planned)
Visual regression testing via GTest-controlled checkpoint execution.

**Workflow** (future `integration-test.yml`):
- Programmatic control via GTest/GMock
- Multi-checkpoint screenshot capture
- Pixel-level comparison against reference images (ImageMagick)
- Artifact upload on regression detection

**Purpose**: Ensure UI renders correctly by detecting visual regressions through automated screenshot comparison.

**Documentation**: See `openspec/specs/display-integration-test/spec.md` for detailed requirements.

