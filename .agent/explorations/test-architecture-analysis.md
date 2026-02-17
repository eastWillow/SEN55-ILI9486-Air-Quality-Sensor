# Test Architecture Analysis: Redundancy & Bad Smells

**Date**: 2026-02-17  
**Issue**: `emulator_test.cpp` spawns binary via `system()` - duplicates smoke test

## The Problem: Two Tests, Same Thing

### Current State

```
┌─────────────────────────────────────────────────────────┐
│ emulator.yml (Smoke Test)                               │
│ ─────────────────────────────────────────               │
│                                                         │
│  xvfb-run ./DisplayEmulator --test                     │
│  ✅ Tests: "Can the binary run without crashing?"      │
│  ✅ Validates: SDL init, basic execution               │
└─────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────┐
│ tests.yml → emulator_test.cpp                          │
│ ────────────────────────────                            │
│                                                         │
│  RunCommand("../DisplayEmulator --test")               │
│  ⚠️  Tests: ALSO "Can the binary run?"                 │
│  ⚠️  Problem: DUPLICATE TEST!                          │
│  ⚠️  Bad Smell: Spawning via system()                  │
└─────────────────────────────────────────────────────────┘
```

## The Bad Smell: `system()` in Tests

```cpp
// emulator_test.cpp:41 - CURRENT (BAD)
int result = RunCommand("../DisplayEmulator --test");
```

**Why this is bad:**
1. **Fragile**: Hardcoded path `../DisplayEmulator` assumes specific build layout
2. **Redundant**: `emulator.yml` already tests binary execution
3. **Wrong abstraction**: Tests should test CODE, not executables
4. **Not portable**: Path breaks if build structure changes
5. **Slow**: Spawning processes is slower than linking libraries

## The Architecture We Already Have

```cmake
# CMakeLists.txt:35 - We have a testable library!
add_library(CoreLib STATIC ${LIB_SOURCES})
    ├── App.cpp
    ├── SensorMock.cpp
    ├── LCD_Driver_SDL.cpp
    ├── LCD_GUI.cpp
    ├── LCD_Touch.cpp
    └── DEV_Config.cpp

# CMakeLists.txt:61 - Binary just wraps library
add_executable(DisplayEmulator main_pc.cpp)
target_link_libraries(DisplayEmulator PRIVATE CoreLib)
```

**CoreLib exists but tests don't use it!**

## What Should Be Tested Where

### Smoke Test (emulator.yml)
**Purpose**: Integration - "Does the final binary work?"
```yaml
- name: Run Emulator Test
  run: xvfb-run ./DisplayEmulator --test
```
**Tests**: 
- Binary builds successfully
- SDL initializes in headless environment
- No crashes during execution
- Exit code 0

### Unit/Component Tests (tests.yml → emulator_test.cpp)
**Purpose**: Component validation - "Does each piece work correctly?"
```cpp
// What it SHOULD be:
#include "App.h"
#include "SensorMock.h"

TEST(AppTest, InitializesCorrectly) {
    App app;
    EXPECT_NO_THROW(app.initialize());
}

TEST(SensorMockTest, ReturnsExpectedData) {
    SensorMock sensor;
    auto data = sensor.readSensorData();
    EXPECT_GT(data.temperature, 0);
}
```

**Tests**:
- App logic correctness
- Sensor data handling
- Display rendering functions
- Touch input processing

## The Fix Path

### Immediate (Quick Fix)
```yaml
# tests.yml - ADD xvfb-run
- name: Run Tests
  run: xvfb-run ctest --output-on-failure
```

### Proper Fix (Architectural)

**Step 1**: Update `tests/CMakeLists.txt`
```cmake
# Current (Line 41-42)
target_link_libraries(emulator_test
    GTest::gtest_main
)

# Should be:
target_link_libraries(emulator_test
    GTest::gtest_main
    CoreLib  # ← Link the library, test the code!
)
```

**Step 2**: Refactor `emulator_test.cpp`
```cpp
// Remove this:
int result = RunCommand("../DisplayEmulator --test");

// Replace with actual component tests:
#include "App.h"
TEST(AppTest, ComponentBehavior) {
    // Test actual code
}
```

**Step 3**: Update specs to reflect separation of concerns

## Testing Strategy Comparison

| Aspect | Smoke Test | Unit/Component Test |
|--------|------------|---------------------|
| **Target** | DisplayEmulator binary | CoreLib components |
| **Method** | Run executable | Link & call functions |
| **Scope** | Integration | Isolated units |
| **Speed** | Slower (~5s) | Faster (~1s) |
| **Fragility** | High (full stack) | Low (isolated) |
| **Debugging** | Hard (black box) | Easy (white box) |
| **CI Workflow** | emulator.yml | tests.yml |

## Dependency on xvfb-run

Both need it because:
```
Smoke Test:
  DisplayEmulator uses SDL2 → needs display → needs xvfb-run ✅

Component Tests:
  CoreLib contains LCD_Driver_SDL.cpp → uses SDL2 → needs xvfb-run ✅
```

**But for different reasons:**
- Smoke test: Full application initialization
- Component tests: Individual SDL function calls

## Future: Visual Regression Tests

When implemented (see `display-integration-test` spec):
```
Third layer:
├── Smoke test: Does it run?
├── Unit tests: Do components work?
└── Visual tests: Does it render correctly?
    └── Screenshot comparison with ImageMagick
```

## Action Items

- [ ] Immediate: Add `xvfb-run` to tests.yml
- [ ] Update ci-pipeline spec with xvfb requirements  
- [ ] Refactor emulator_test.cpp to test CoreLib components
- [ ] Update test-independence spec with proper architecture
- [ ] Document testing strategy in project README
