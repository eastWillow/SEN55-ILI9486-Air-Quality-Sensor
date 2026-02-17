## 1. Deterministic Rendering Validation

- [x] 1.1 Audit `App_Loop()`, `LCD_GUI.cpp`, and `LCD_Touch.cpp` for calls to `time()`, `SDL_GetTicks()`, `rand()`, or other non-deterministic functions
- [x] 1.2 Replace any wall-clock dependencies with injectable `Clock` abstraction (real clock for production, fixed clock for test mode)
- [x] 1.3 Seed or eliminate any animation randomness in test mode
- [x] 1.4 Verify determinism: run `./DisplayEmulator --test` 5 times and confirm `md5sum screenshot.bmp` is identical each time

## 2. EmulatorEngine Test API

- [x] 2.1 Create `src/App/EmulatorEngine.h` and `src/App/EmulatorEngine.cpp` with `initialize()`, `stepFrames()`, `captureScreenshot()`, and `shutdown()` methods
- [x] 2.2 Add `EmulatorEngine` to CoreLib in the root `CMakeLists.txt`
- [x] 2.3 Verify `main_pc.cpp` still compiles and works unchanged (EmulatorEngine wraps existing functions, doesn't replace them)

## 3. Reference Image Generation

- [x] 3.1 Run emulator locally in test mode, visually inspect output at frame 0 (startup) and frame 25 (final)
- [x] 3.2 Create `tests/integration/reference_screenshots/` directory
- [x] 3.3 Generate and commit `startup.bmp` (frame 0) and `final.bmp` (frame 25) as golden references
- [x] 3.4 Create `tests/integration/reference_screenshots/README.md` documenting the update process

## 4. GTest Integration Test Suite

- [x] 4.1 Create `tests/integration/display_integration_test.cpp` with `DisplayIntegrationTest` fixture using `EmulatorEngine`
- [x] 4.2 Implement `CheckpointStartup` test (frame 0 vs `startup.bmp`)
- [x] 4.3 Implement `CheckpointFinal` test (frame 25 vs `final.bmp`)
- [x] 4.4 Implement `CompareWithReference()` helper using ImageMagick `compare -metric AE` with configurable pixel threshold
- [x] 4.5 Update `tests/CMakeLists.txt` to build the integration test suite and link CoreLib

## 5. CI Workflow

- [x] 5.1 Create `.github/workflows/integration-test.yml` running on `ubuntu-latest`
- [x] 5.2 Configure workflow to install dependencies (`libsdl2-dev`, `imagemagick`, `xvfb`) and run `ctest`
- [x] 5.3 Configure artifact upload for failed tests (uploading `test_output/actual_*.bmp` and `test_output/diff_*.bmp`)
- [x] 5.4 Verify workflow runs independently of existing unit tests (or integrates cleanly)ator.yml`

## 6. Documentation

- [x] 6.1 Update `README.md` in root to mention integration tests and how to run them locally
- [x] 6.2 Add integration test section to `README.md` with local run instructions
- [x] 6.3 Update `openspec/specs/software-architecture.md` testing strategy to mark integration tests as implemented
