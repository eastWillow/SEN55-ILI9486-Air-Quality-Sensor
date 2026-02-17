## Why

Tests using SDL2 require a virtual display in headless CI environments, but `tests.yml` currently runs `ctest` without `xvfb-run`, causing failures. Additionally, `emulator_test.cpp` spawns the DisplayEmulator binary via `system()`, duplicating the smoke test in `emulator.yml` rather than testing CoreLib components directly. This creates test fragility, redundancy, and limits debugging granularity.

## What Changes

- Add `xvfb-run` to the `tests.yml` CI workflow for all test execution commands
- Refactor `emulator_test.cpp` to link and test CoreLib components instead of spawning the binary
- Update `tests/CMakeLists.txt` to link CoreLib into test executable
- Document xvfb-run requirements and testing strategy in specs
- Remove redundant binary spawning from tests (eliminate `system()` calls)

## Capabilities

### New Capabilities
- `test-display-requirements`: Requirements for running SDL2-based tests in headless CI environments

### Modified Capabilities
- `ci-pipeline`: Add headless display support requirements for graphics-based tests
- `test-independence`: Add CI environment compatibility and test layer separation requirements
- `emulator-split`: Clarify distinction between smoke tests (binary) and component tests (CoreLib)

## Impact

**Modified Files**:
- `.github/workflows/tests.yml` - Add xvfb-run wrapper
- `tests/CMakeLists.txt` - Link CoreLib library
- `tests/emulator_test.cpp` - Refactor from binary spawning to component testing
- `openspec/specs/ci-pipeline/spec.md` - Add display requirements
- `openspec/specs/test-independence/spec.md` - Add environment requirements
- `openspec/specs/emulator-split/spec.md` - Clarify test separation

**Testing Impact**:
- Component tests become more granular and debuggable
- Eliminates redundancy between smoke tests and unit tests
- Establishes clear testing pyramid: smoke (binary) → component (CoreLib) → unit (logic)
