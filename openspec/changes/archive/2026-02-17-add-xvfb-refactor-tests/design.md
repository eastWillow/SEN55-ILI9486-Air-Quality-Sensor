## Context

**Current State**:
- `tests.yml` runs `ctest` without `xvfb-run`, causing SDL2 initialization failures in headless CI
- `emulator_test.cpp` spawns DisplayEmulator binary via `system("../DisplayEmulator --test")`
- This duplicates the smoke test in `emulator.yml` rather than testing CoreLib components
- CoreLib exists as a static library but tests don't link it

**Background**:
- DisplayEmulator architecture: CoreLib (library) + main_pc.cpp (entry point)
- CoreLib contains: App.cpp, SensorMock.cpp, LCD_Driver_SDL.cpp, LCD_GUI.cpp, etc.
- emulator.yml provides smoke testing (binary-level integration)
- tests.yml should provide component testing (library-level validation)

**Constraints**:
- Must maintain backward compatibility with main project build
- SDL2 requires X11 display - Xvfb provides virtual framebuffer in CI
- Cannot break existing smoke test workflow

## Goals / Non-Goals

**Goals:**
- Add xvfb-run to tests.yml for SDL2 compatibility
- Refactor emulator_test.cpp to test CoreLib components directly
- Eliminate redundancy between smoke tests and component tests
- Improve test granularity and debugging capability
- Document xvfb-run mental model for future developers

**Non-Goals:**
- Modifying emulator.yml smoke test workflow
- Implementing visual regression testing (see display-integration-test)
- Adding new test cases beyond architectural refactoring
- Changes to CoreLib itself (only test infrastructure)

## Decisions

### Decision 1: Add xvfb-run at CTest Level (Not Per-Test)
**Choice**: Wrap `ctest` command with `xvfb-run` in tests.yml  
**Alternatives Considered**:
1. ❌ Add xvfb-run to individual test CMake commands - Too fragile, duplicative
2. ❌ Use GitHub Actions display action - Extra dependency, unnecessary complexity
3. ✅ Wrap ctest invocation - Simple, child processes inherit DISPLAY

**Rationale**: Single point of control, covers all current and future tests automatically.

### Decision 2: Link CoreLib Instead of Spawning Binary
**Choice**: Refactor emulator_test.cpp to link CoreLib and test components  
**Alternatives Considered**:
1. ❌ Keep system() spawning, just add xvfb-run - Maintains redundancy and fragility
2. ❌ Remove emulator_test.cpp entirely - Loses component test capability
3. ✅ Link CoreLib library - Proper abstraction, enables granular testing

**Rationale**: 
- Eliminates hardcoded path fragility (`../DisplayEmulator`)
- Enables white-box testing of components
- Faster test execution (no process spawning)
- Clear separation: smoke test (binary) vs component test (library)

### Decision 3: Keep Existing Smoke Test Unchanged
**Choice**: Leave emulator.yml as-is, it serves a different purpose  
**Rationale**: Smoke test validates binary integration; component tests validate logic. Both are needed.

### Decision 4: Document Mental Model in Specs
**Choice**: Create test-display-requirements capability with clear xvfb-run rules  
**Rationale**: Future developers need to understand SDL → display → xvfb-run relationship.

## Implementation Approach

### Phase 1: Add xvfb-run (Immediate Fix)
```yaml
# .github/workflows/tests.yml
- name: Run Tests
  working-directory: build_tests
  run: xvfb-run ctest --output-on-failure  # ← Add xvfb-run wrapper
```

### Phase 2: Refactor Test Architecture
```cmake
# tests/CMakeLists.txt
target_link_libraries(emulator_test
    GTest::gtest_main
    CoreLib  # ← Add CoreLib link
)
```

```cpp
// tests/emulator_test.cpp - Replace system() with component tests
#include "App.h"
#include "SensorMock.h"

TEST(AppTest, InitializesCorrectly) {
    App app;
    EXPECT_NO_THROW(app.initialize());
}

TEST(SensorMockTest, ProvidesData) {
    SensorMock sensor;
    auto data = sensor.readSensorData();
    EXPECT_GT(data.temperature, 0);
}
```

### Phase 3: Update Specs
- Add test-display-requirements capability
- Update ci-pipeline, test-independence, emulator-split with xvfb-run requirements

## Risks / Trade-offs

**Risk**: CoreLib may not be designed for standalone testing  
→ **Mitigation**: Start with simple initialization tests, expand as needed. CoreLib already compiles as library.

**Risk**: Breaking test backward compatibility  
→ **Mitigation**: Phase implementation - add xvfb-run first (non-breaking), then refactor tests.

**Trade-off**: More initial setup complexity  
→ **Benefit**: Better long-term maintainability, clearer test boundaries, faster debugging.

**Risk**: Need to make CoreLib components more testable  
→ **Mitigation**: Accepted - this is good design pressure. Refactor components if needed.

## Migration Plan

1. **Immediate**: Add xvfb-run to tests.yml (fixes CI failures)
2. **Refactor**: Update CMakeLists.txt to link CoreLib
3. **Replace Tests**: Convert emulator_test.cpp from binary spawning to component testing
4. **Verify**: Ensure both emulator.yml and tests.yml pass in CI
5. **Document**: Update specs with new requirements

**Rollback Strategy**: Remove xvfb-run if issues, revert to system() spawning if CoreLib linking fails.

## Open Questions

- ~~Should we keep any binary spawn tests?~~ → No, that's emulator.yml's job
- ~~What CoreLib components should we test first?~~ → App initialization, SensorMock
- Do we need to refactor CoreLib API for better testability? → Defer, evaluate after initial tests
