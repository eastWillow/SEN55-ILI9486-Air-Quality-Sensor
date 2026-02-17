## ADDED Requirements

### Requirement: Distinction from Component Tests
The smoke test workflow MUST be clearly separated from component/unit test workflows.

#### Scenario: Test Layer Responsibilities
- **WHEN** Evaluating test coverage
- **THEN** Smoke tests (emulator.yml) validate binary integration
- **AND** Component tests (tests.yml) validate CoreLib correctness
- **AND** Both test types serve distinct purposes without duplication

**Test Comparison**:

| Test Type | Smoke Test (emulator.yml) | Component/Unit Tests (tests.yml) |
|-----------|--------------------------|----------------------------------|
| **Target** | DisplayEmulator binary | CoreLib components |
| **Method** | Execute binary | Link library & call functions |
| **Purpose** | "Can it run?" | "Does it work correctly?" |
| **Scope** | Full integration | Isolated components |
| **xvfb-run needed?** | ✅ Yes (runs SDL2 app) | ✅ Yes (CoreLib uses SDL2) |

#### Scenario: Both Need xvfb-run
- **WHEN** Running smoke tests
- **THEN** xvfb-run is required because DisplayEmulator calls SDL2_Init()
- **WHEN** Running component tests
- **THEN** xvfb-run is required because CoreLib contains LCD_Driver_SDL.cpp

**Rationale**: Both test types use SDL2, but at different abstraction levels - smoke test exercises the full application, component tests validate individual SDL function calls.

### Requirement: Test Coverage Documentation
Documentation MUST clarify what each test type validates.

#### Scenario: Smoke Test Validation
- **WHEN** emulator.yml executes
- **THEN** It validates: Binary builds, SDL initializes, No crashes, Exit code 0
- **AND** It does NOT validate: Component behavior, Business logic correctness

#### Scenario: Component Test Validation  
- **WHEN** tests.yml executes
- **THEN** It validates: App initialization, Sensor data processing, Display functions, Error handling
- **AND** It does NOT validate: Final binary integration (that's smoke test)

**Testing Pyramid**:
```
    △ emulator.yml (Smoke - Binary Integration)
   △△ Visual regression (Future - Screenshot comparison)
  △△△ tests.yml (Component - CoreLib validation)
```
