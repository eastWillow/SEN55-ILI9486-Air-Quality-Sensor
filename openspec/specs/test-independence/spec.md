## ADDED Requirements

### Requirement: Standalone Test Execution
The `tests` CMake project must be configurable and buildable independently from the main project.

#### Scenario: Running Tests from Subdirectory
- **WHEN** A developer navigates to the `tests` directory and runs `cmake -S . -B build`
- **THEN** CMake successfully configures the project without errors related to parent scope variables or missing targets.
- **AND** Running `cmake --build build` compiles the test executable.
- **AND** Running `ctest` executes the tests successfully.

### Requirement: Resource Independence
Tests must locate necessary resources (e.g., reference images) relative to the test executable or source directory, without relying on `file(COPY)` commands that assume a specific build artifact structure from the parent project.

#### Scenario: Verify Resource Loading
- **WHEN** Tests are executed from the build directory.
- **THEN** The test runner correctly locates `reference/` images required for visual regression tests.

### Requirement: CI Environment Compatibility
Tests MUST execute successfully in headless CI environments.

#### Scenario: Graphics Tests in Headless Environment
- **WHEN** Tests include SDL2, OpenGL, or other display-dependent code
- **THEN** The CI workflow MUST provide a virtual display via `xvfb-run`
- **AND** Test executables can initialize graphics libraries
- **AND** All spawned child processes inherit the display environment

### Requirement: Test Layer Separation
Tests MUST target the appropriate abstraction layer to avoid redundancy.

#### Scenario: Avoiding Smoke Test Duplication
- **GIVEN** A smoke test workflow exists for binary validation
- **WHEN** Writing unit/component tests
- **THEN** Tests MUST link against `CoreLib`, not execute the binary via `system()`
- **AND** Tests validate component behavior, not binary integration
- **AND** There is no duplication between smoke tests and unit tests

#### Scenario: Component Testing Strategy
- **WHEN** Testing `CoreLib` functionality
- **THEN** Tests MUST link the `CoreLib` library
- **AND** Tests call functions directly
- **AND** Tests do NOT spawn the `DisplayEmulator` binary
- **AND** `CMakeLists.txt` includes `target_link_libraries(test_target CoreLib)`

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

#### Scenario: Proper Test Granularity
- **WHEN** A test failure occurs
- **THEN** The failing test MUST clearly identify the broken component
- **AND** Developers can debug at the appropriate abstraction level
- **AND** Binary-level failures are caught by smoke tests
- **AND** Component-level failures are caught by unit tests

**Rationale**: 
- Binary spawning via `system()` is fragile (hardcoded paths, brittle)
- Duplicates smoke test functionality
- Limits test granularity and debugging capability
- Increases test execution time
