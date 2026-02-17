# Learning: xvfb-run = GUI = SDL

**Date**: 2026-02-17  
**Context**: Refining test infrastructure requirements

## The Mental Model

```
When I see:           Then I think:              So in CI I need:
────────────────────────────────────────────────────────────────
SDL2 dependency   →   GUI/Graphics rendering →   xvfb-run
OpenGL            →   GPU rendering          →   xvfb-run  
X11 window        →   Display required       →   xvfb-run
Headless CI       →   No physical display    →   Virtual framebuffer
```

## The Rule

**If SDL (or any GUI library) → Needs display → Needs xvfb-run in CI**

## When NOT Needed

```
✗ Compile-only checks (arduino.yml - just verify code compiles)
✗ Publishing artifacts (wasm.yml - building output, not testing rendering)
✗ Pure logic tests (no graphics, just algorithms)
```

## How xvfb-run Works

```
┌──────────────────────────────────────────────┐
│ CI Environment (Headless - No Display)       │
├──────────────────────────────────────────────┤
│                                              │
│  xvfb-run <command>                          │
│      │                                       │
│      ├─ Starts Xvfb (X Virtual FrameBuffer) │
│      ├─ Sets DISPLAY=:99 (or similar)       │
│      ├─ Runs <command>                       │
│      └─ Kills Xvfb when done                 │
│                                              │
│  All child processes inherit DISPLAY env     │
│  SDL2 can render to virtual framebuffer      │
│                                              │
└──────────────────────────────────────────────┘
```

## Process Inheritance

```
xvfb-run ctest
    └── Runs test binary (inherits DISPLAY)
        └── Spawns DisplayEmulator via system()
            └── Also inherits DISPLAY ✅
                SDL2_Init() succeeds
```

**Key insight**: You only need xvfb-run at the TOP level command. All children inherit it.

## Application to This Project

| Workflow | Tests What? | Needs xvfb-run? | Why? |
|----------|-------------|-----------------|------|
| `emulator.yml` | DisplayEmulator binary | ✅ Yes | Uses SDL2 for rendering |
| `tests.yml` | CoreLib + integration | ✅ Yes | Tests spawn SDL2 apps |
| `arduino.yml` | Arduino compilation | ❌ No | Compile-only, no execution |
| `wasm.yml` | Build & publish WASM | ❌ No | Building artifacts, not testing |

## Commands That Need It

```bash
# ✅ Correct
xvfb-run ./DisplayEmulator --test
xvfb-run ctest --output-on-failure

# ❌ Wrong (will fail in headless CI)
./DisplayEmulator --test
ctest --output-on-failure
```

## References

- SDL2 requires X11 display: https://wiki.libsdl.org/SDL2/FAQLinux
- Xvfb documentation: https://www.x.org/releases/X11R7.6/doc/man/man1/Xvfb.1.xhtml
- GitHub Actions headless testing: https://github.com/marketplace/actions/run-headless-display-action
