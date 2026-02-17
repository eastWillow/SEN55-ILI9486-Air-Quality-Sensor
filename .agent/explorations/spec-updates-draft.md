# Spec Updates Draft: xvfb-run Requirements

**Date**: 2026-02-17  
**Goal**: Ensure all SDL-based tests have proper display environment requirements

## 1. Update `specs/ci-pipeline/spec.md`

### Add Section: Headless Display Support

```markdown
### Requirement: Headless Display Support
The CI/CD test workflow must provide a virtual display for graphics-based tests.

#### Scenario: Running SDL2 Tests in CI
- **WHEN** The tests workflow is triggered in GitHub Actions
- **THEN** The test runner is invoked under xvfb-run
- **AND** Tests requiring display access (SDL2, OpenGL, GUI) execute successfully
- **AND** All child processes inherit the virtual display environment
- **AND** SDL2 initialization completes without errors

**Rationale**: SDL2 applications require an X11 display. In headless CI environments, Xvfb (X Virtual FrameBuffer) provides a virtual display server.

**Implementation**:
```yaml
- name: Run Tests
  run: xvfb-run ctest --output-on-failure
```

#### Scenario: Non-Graphics Tests
- **WHEN** Tests do not use graphics libraries (e.g., pure logic, compile-only checks)
- **THEN** xvfb-run is NOT required
- **AND** Tests execute directly without virtual display overhead

**Examples**:
- ✅ Needs xvfb-run: SDL2-based tests, OpenGL rendering tests
- ❌ Doesn't need: Arduino compilation checks, pure unit tests, WASM builds
```

---

## 2. Update `specs/test-independence/spec.md`

### Add Section: Test Environment Requirements

```markdown
### Requirement: CI Environment Compatibility
Tests must execute successfully in headless CI environments.

#### Scenario: Graphics Tests in Headless Environment
- **WHEN** Tests include SDL2, OpenGL, or other display-dependent code
- **THEN** The CI workflow provides a virtual display via xvfb-run
- **AND** Test executables can initialize graphics libraries
- **AND** All spawned child processes inherit the display environment

#### Scenario: Test Isolation from Build Environment
- **WHEN** Tests are executed via CTest
- **THEN** Tests link against CoreLib components directly
- **AND** Tests do NOT spawn the final binary via system() calls
- **AND** Tests validate component behavior, not binary integration

**Anti-pattern to avoid**:
```cpp
// ❌ BAD: Spawning binary in tests
RunCommand("../DisplayEmulator --test");
```

**Correct approach**:
```cpp
// ✅ GOOD: Testing components directly
#include "App.h"
TEST(AppTest, ComponentBehavior) {
    App app;
    EXPECT_NO_THROW(app.initialize());
}
```

**Rationale**: 
- Binary spawning is fragile (hardcoded paths)
- Duplicates smoke tests
- Limits test granularity
- Makes debugging harder
```

---

## 3. Update `specs/emulator-split/spec.md`

### Clarify Scope vs tests.yml

Add this section after "Distinction from Integration Tests":

```markdown
## Distinction from Unit Tests

The smoke test (`emulator.yml`) is separate from component tests (`tests.yml`):

| Test Type | Smoke Test | Component/Unit Tests |
|-----------|------------|---------------------|
| **Workflow** | `emulator.yml` | `tests.yml` |
| **Target** | DisplayEmulator binary | CoreLib components |
| **Method** | Execute binary | Link library & call functions |
| **Purpose** | "Can it run?" | "Does it work correctly?" |
| **Scope** | Full integration | Isolated components |
| **xvfb-run needed?** | ✅ Yes (runs SDL2 app) | ✅ Yes (CoreLib uses SDL2) |

### Both Need xvfb-run, But For Different Reasons

**Smoke Test (`emulator.yml`)**:
```bash
xvfb-run ./DisplayEmulator --test
# Needs display: Full application SDL2_Init()
```

**Component Tests (`tests.yml`)**:
```bash
xvfb-run ctest --output-on-failure
# Needs display: CoreLib contains LCD_Driver_SDL.cpp functions
```

### What Each Tests

**Smoke Test**: Binary-level integration
- Binary builds successfully
- SDL initializes in headless environment
- No crashes during full execution
- Exit code 0

**Component Tests**: Code-level correctness
- App initialization logic
- Sensor data processing
- Display rendering functions
- Touch input handling
- Error conditions and edge cases
```

---

## 4. New Requirement: Test Layer Separation

### Add to `specs/test-independence/spec.md`

```markdown
### Requirement: Test Layer Separation
Tests must target the appropriate abstraction layer to avoid redundancy.

#### Scenario: Avoiding Smoke Test Duplication
- **GIVEN** A smoke test workflow exists for binary validation
- **WHEN** Writing unit/component tests
- **THEN** Tests must link against CoreLib, not execute the binary
- **AND** Tests validate component behavior, not binary integration
- **AND** There is no duplication between smoke tests and unit tests

#### Scenario: Proper Test Granularity
- **WHEN** A test failure occurs
- **THEN** The failing test clearly identifies the broken component
- **AND** Developers can debug at the appropriate abstraction level
- **AND** Binary-level failures go to smoke tests
- **AND** Component-level failures go to unit tests

**Example Structure**:
```
Testing Pyramid:
    △ emulator.yml (Smoke)
   △△ tests.yml + visual regression (Integration)
  △△△ tests.yml (Unit/Component)
```
```

---

## 5. Quick Reference Table for Workflows

Add to project README or create `docs/testing-strategy.md`:

```markdown
## CI Workflow Testing Strategy

| Workflow | Purpose | xvfb-run? | Why? | Command |
|----------|---------|-----------|------|---------|
| `emulator.yml` | Smoke test binary | ✅ Yes | Uses SDL2 | `xvfb-run ./DisplayEmulator --test` |
| `tests.yml` | Unit/component tests | ✅ Yes | CoreLib uses SDL2 | `xvfb-run ctest --output-on-failure` |
| `arduino.yml` | Compile check | ❌ No | Compilation only | `arduino-cli compile` |
| `wasm.yml` | Build & publish | ❌ No | Build artifacts | `emcmake cmake ...` |

### Mental Model

**Rule**: If you see SDL2 (or any GUI library) → think "needs display" → think "needs xvfb-run in CI"

```
SDL2 dependency  →  GUI/Graphics  →  xvfb-run required
Compile-only     →  No execution  →  No xvfb-run needed
```
```

---

## Summary of Changes

### Immediate Fixes
1. **tests.yml**: Change `ctest` to `xvfb-run ctest`
2. **Document mental model**: xvfb-run = SDL = GUI

### Spec Updates
1. **ci-pipeline/spec.md**: Add "Headless Display Support" requirement
2. **test-independence/spec.md**: Add "CI Environment Compatibility" + anti-patterns
3. **emulator-split/spec.md**: Clarify distinction from unit tests
4. Add "Test Layer Separation" requirement

### Future Work
1. Refactor `emulator_test.cpp` to link CoreLib instead of spawning binary
2. Update `tests/CMakeLists.txt` to link CoreLib
3. Write actual component tests for App, SensorMock, LCD_Driver_SDL
4. Document testing strategy in README
