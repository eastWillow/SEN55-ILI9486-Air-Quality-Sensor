# Tasks: split-emulator-workflows

## 1. Core Refactoring

- [x] 1.1 **Refactor Main Loop**: Extract loop logic from `main()` to `RunApp(bool infinite)`. Ensure `LCD_Quit()` is called correctly in both paths. <!-- id: 0 -->
- [x] 1.2 **Add CLI Args**: Update `main_pc.cpp` to parse `--test` arg. Default to infinite loop if no args provided. <!-- id: 1 -->

## 2. Test Implementation

- [x] 2.1 **Implement GTest Harness**: Create `tests/emulator_test.cpp` (or similar) that launches `./main_pc --test`, waits for exit, loads `screenshot.bmp`, and compares it against a reference BMP. <!-- id: 2 -->
- [x] 2.2 **Add Reference Image**: Create/add `tests/reference/golden_screen.bmp` for the test comparison. <!-- id: 3 -->

## 3. Integration

- [x] 3.1 **Update CI/CD**: Modify `.github/workflows/emulator.yml` (and/or `tests.yml`) to run the new GTest suite. Ensure build artifact retention for failure debugging. <!-- id: 4 -->
