## Context

The project has a two-tier testing architecture. Tier 1 (smoke test via `emulator.yml`) validates that the DisplayEmulator binary can run without crashing. Tier 2 (visual regression via `integration-test.yml`) is fully specified in `display-integration-test/spec.md` but not yet implemented.

The emulator currently runs as a monolithic `main()` function in `main_pc.cpp` that:
1. Calls `App_Setup(&mockSensor)` once
2. Loops `App_Loop(&mockSensor)` + `LCD_Update()` (25 iterations in `--test` mode)
3. Saves a single screenshot at the end via `LCD_SaveScreenshot("screenshot.bmp")`

CoreLib already exists as a static library (`App.cpp`, `SensorMock.cpp`, `LCD_Driver_SDL.cpp`, `LCD_GUI.cpp`, etc.) — the binary just wraps it. This is the hook point for testability.

## Goals / Non-Goals

**Goals:**
- Enable multi-checkpoint screenshot capture during test execution
- Guarantee deterministic (byte-identical) rendering across runs
- Pixel-level comparison against committed reference images
- Independent CI workflow that runs in parallel with smoke test
- Clear process for updating reference images when UI intentionally changes

**Non-Goals:**
- Adaptive per-checkpoint thresholds (future enhancement)
- HTML visual diff reports (future enhancement)
- Automated baseline update via PR bots (future enhancement)
- Cross-platform rendering tests (future enhancement)
- Replacing or modifying the existing smoke test

## Decisions

### 1. Emulator Testability: Library-Level Control via `EmulatorEngine`

**Decision**: Create an `EmulatorEngine` class that wraps the init/loop/screenshot lifecycle, exposable to GTest.

**Why not just shell out to the binary?**
- Fragile paths (`../DisplayEmulator` breaks on build layout changes)
- No mid-execution checkpoints (binary only captures final frame)
- Duplicates smoke test (binary execution is already tested)
- Process spawning is slower than in-process calls

**Approach**:
```cpp
// src/App/EmulatorEngine.h
class EmulatorEngine {
public:
    void initialize(SensorIntf* sensor);  // App_Setup wrapper
    void stepFrames(int count);            // N iterations of App_Loop + LCD_Update
    void captureScreenshot(const std::string& path);  // LCD_SaveScreenshot wrapper
    void shutdown();                       // LCD_Quit wrapper
};
```

This class lives in CoreLib so GTest links against it directly. The existing `main_pc.cpp` continues to work unchanged — it just calls the same underlying functions.

**Alternatives considered**:
- *Fork the binary and inject checkpoints via CLI args*: Too fragile, no multi-checkpoint support
- *Use SDL2 render-to-texture for in-memory comparison*: Over-engineered for BMP comparison

### 2. Determinism: Injectable Clock + Seeded Mock Data

**Decision**: `SensorMock` already returns fixed data (good). Time-dependent code must use an injectable clock instead of `time()`.

**Approach**:
- Audit `App_Loop()` for any calls to `time()`, `SDL_GetTicks()`, or similar
- Replace with a `Clock` abstraction (real clock for production, fixed clock for tests)
- Verify: run `--test` mode 5 times, `md5sum screenshot.bmp` must be identical each time

**If animations exist**: Seed the RNG or freeze animation state in test mode.

### 3. Image Comparison: ImageMagick `compare -metric AE`

**Decision**: Use ImageMagick's `compare` tool with Absolute Error pixel metric.

**Why ImageMagick?**
- Industry standard, available via `apt-get`
- Generates visual diff images automatically
- Configurable thresholds
- Already installed in CI environments or trivially installable

**Threshold**: Start with 0 (byte-identical). If anti-aliasing or platform variance causes issues, relax to ≤100 pixel difference.

**Comparison wrapper**:
```cpp
void CompareWithReference(const std::string& actual, const std::string& reference) {
    std::string cmd = "compare -metric AE " + actual + " " + reference + " diff_" + actual;
    int diff = RunImageMagickCompare(cmd);
    ASSERT_LE(diff, PIXEL_THRESHOLD) << "Visual regression detected: " << diff << " pixels differ";
}
```

**Alternatives considered**:
- *Raw byte comparison*: Too strict, breaks on any sub-pixel rendering variance
- *OpenCV*: Heavyweight dependency for simple BMP comparison
- *Custom pixel diff*: Reinventing the wheel

### 4. Checkpoint Strategy: 2 Checkpoints (Startup + Final)

**Decision**: Start with 2 checkpoints. Add more only if regressions slip through.

| Checkpoint | Frame | Validates |
|-----------|-------|-----------|
| `startup` | 0 | Layout renders, initial state correct |
| `final` | 25 | Sensor data displayed, full UI rendered |

**Why not 3+ checkpoints?**
- More checkpoints = more reference images to maintain
- More reference images = more merge conflicts when UI changes
- Start minimal, add checkpoints only when a regression demands it

### 5. Reference Image Storage: Git-Tracked in Repository

**Decision**: Store reference images in `tests/integration/reference_screenshots/`.

```
tests/integration/
├── reference_screenshots/
│   ├── README.md           ← How to update references
│   ├── startup.bmp         ← Checkpoint 0 golden image
│   └── final.bmp           ← Checkpoint 25 golden image
└── display_integration_test.cpp
```

**Update process**:
1. Run emulator locally → visually inspect screenshots
2. Copy to `reference_screenshots/` → commit with message explaining the UI change
3. CI uses committed references for comparison

### 6. CI Workflow: Independent, Parallel with Smoke Test

**Decision**: New `.github/workflows/integration-test.yml` that runs independently.

**Why independent?**
- Smoke test is fast (~5s) — no point blocking on it
- Different failure modes: smoke catches crashes, integration catches rendering bugs
- Parallel execution = faster total CI time
- Redundant failure on shared issues is acceptable

## Risks / Trade-offs

**Reference image staleness** → Mitigation: Document update process clearly in `tests/integration/reference_screenshots/README.md`. CI failure forces review.

**Non-deterministic rendering across environments** → Mitigation: Validate byte-identical output locally first (Phase 1). If CI differs from local, investigate SDL2 platform rendering. Relax threshold as last resort.

**Binary files in Git** → Mitigation: BMP files for a 320×480 display are ~450KB each. Two checkpoints = ~1MB. Acceptable. If growth becomes an issue, switch to Git LFS.

**Maintenance burden of reference images** → Mitigation: Start with only 2 checkpoints. Each intentional UI change requires reference update — this is a feature (forces visual review), not a bug.

**EmulatorEngine refactor risk** → Mitigation: The refactor wraps existing functions (`App_Setup`, `App_Loop`, `LCD_Update`, `LCD_SaveScreenshot`). No logic changes — just API surface for test control. Existing `main_pc.cpp` continues to call the same functions.
