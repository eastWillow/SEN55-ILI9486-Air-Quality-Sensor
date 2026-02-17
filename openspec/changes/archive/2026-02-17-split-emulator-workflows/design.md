# Design: split-emulator-workflows

## Context

The current `main_pc.cpp` runs a fixed 25-frame loop (approx 1.25s), takes a screenshot, and exits. This supports a primitive form of "Workflow A" (automated test) but prevents "Workflow B" (interactive user exploring), which needs to run indefinitely.

## Goals / Non-Goals

**Goals:**
*   Support a dedicated **Test Mode** that runs headlessly/controlled, injects inputs, runs for strictly N frames, saves output, and exits.
*   Support a dedicated **Interactive Mode** that runs indefinitely and accepts user inputs via SDL.
*   Ensure the Test Mode can be driven by a Google Test harness for CI/CD verification.

**Non-Goals:**
*   Full GUI automated testing framework (we only need basic screenshot comparison for now).
*   Changing the core extensive sensor logic (we mock it).

## Decisions

### 1. Refactor Main Loop
Decouple the loop logic from `main()`. Create a `RunApp(bool infinite)` function (or similar) that can be called with different parameters.
*   `main()` will parse arguments and call `RunApp`.
*   `RunApp` will handle the loop, whether finite or infinite.

### 2. CLI Argument Parsing
Update `main()` to check `argc`/`argv`.
*   If `./main_pc --test`: Enter `TEST_FIXED` mode (25 frames).
*   Otherwise: Enter `INTERACTIVE` mode (infinite loop).

### 3. GTest Controller (Test Workflow)
Create `tests/emulator_test.cpp`.
*   It spawns `./main_pc --test`.
*   Waits for exit.
*   Loads `screenshot.bmp` and compares it to `tests/reference/golden_screen.bmp`.

### 4. CI/CD Integration
*   Update `.github/workflows/emulator.yml` to run the new GTest target.

## Risks / Trade-offs

*   **Risk**: GTest spawning subprocesses can be flaky across different environments.
    *   *Mitigation*: Ensure robust path handling and timeout logic.
*   **Trade-off**: Slightly more complex `main.cpp` vs duplicated code for two binaries. We choose the single binary approach for simplicity of build maintenance.
