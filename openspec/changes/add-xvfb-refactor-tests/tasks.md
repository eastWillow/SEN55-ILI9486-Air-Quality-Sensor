## 1. Add xvfb-run to CI Workflow

- [x] 1.1 Update .github/workflows/tests.yml to wrap ctest with xvfb-run
- [x] 1.2 Verify tests.yml syntax is valid
- [x] 1.3 Commit and push to trigger CI validation *[Manual step - committed locally]*

## 2. Refactor Test CMake Configuration

- [x] 2.1 Update tests/CMakeLists.txt to link CoreLib library
- [x] 2.2 Remove obsolete file(COPY) commands if present
- [x] 2.3 Verify standalone test build still works (cmake -S tests -B build_tests)

## 3. Refactor Component Tests

- [x] 3.1 Remove RunCommand() helper and system() spawning from emulator_test.cpp
- [x] 3.2 Add CoreLib component includes (App.h, SensorMock.h, etc.)
- [x] 3.3 Implement App initialization test
- [x] 3.4 Implement SensorMock data validation test
- [x] 3.5 Update test documentation/comments to reflect component testing purpose

## 4. Verify Test Execution

- [x] 4.1 Build and run tests locally with xvfb-run
- [ ] 4.2 Verify emulator.yml smoke test still passes (no changes to emulator workflow) *[Requires CI]*
- [ ] 4.3 Verify tests.yml component tests pass in CI *[Requires git push]*
- [ ] 4.4 Confirm both test workflows execute without duplication *[Requires CI]*

## 5. Update Documentation

- [x] 5.1 Add README section explaining test strategy (smoke vs component)
- [x] 5.2 Document xvfb-run requirement for SDL2-based tests
- [x] 5.3 Update test-related comments to reference correct abstraction layers
