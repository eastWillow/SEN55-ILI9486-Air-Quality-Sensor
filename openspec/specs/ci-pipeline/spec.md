## ADDED Requirements

### Requirement: Independent Test Pipeline
The CI/CD test workflow must be able to build tests efficiently using a unified configuration, removing redundant builds across workflows while maintaining standalone execution capabilities.

#### Scenario: Running Tests in CI
- **WHEN** The unified tests workflow is triggered in GitHub Actions
- **THEN** The workflow configures CMake directly from the `tests` subdirectory (or root, whichever is optimal)
- **AND** The build completes successfully exactly once without requiring multiple builds for different test stages
- **AND** All tests execute and pass sequentially in their respective steps (Smoke, CoreLib, GUI, Performance)

### Requirement: Headless Display Support
The CI/CD test workflow MUST provide a virtual display for graphics-based tests.

#### Scenario: Running SDL2 Tests in CI
- **WHEN** The tests workflow is triggered in GitHub Actions
- **THEN** The test runner MUST be invoked under `xvfb-run`
- **AND** Tests requiring display access (SDL2, OpenGL, GUI) execute successfully
- **AND** All child processes inherit the virtual display environment
- **AND** SDL2 initialization completes without errors

#### Scenario: Non-Graphics Tests  
- **WHEN** Tests do not use graphics libraries (pure logic, compile-only checks)
- **THEN** `xvfb-run` MUST NOT be used
- **AND** Tests execute directly without virtual display overhead

**Rationale**: SDL2 applications require an X11 display. In headless CI environments, Xvfb (X Virtual FrameBuffer) provides a virtual display server.

**Implementation**:
```yaml
- name: Run Tests
  run: xvfb-run ctest --output-on-failure
```

### Requirement: Backward Compatibility
The main project build must continue to work with tests as a subdirectory.

#### Scenario: Building from Root
- **WHEN** A developer runs `cmake -S . -B build` from the project root
- **THEN** The main project and all subdirectories (including tests) are configured
- **AND** Running `ctest` executes all tests successfully

### Requirement: GitHub Action Version Standards
The CI/CD workflows MUST use specified versions of standard GitHub Actions to ensure security and stability.

#### Scenario: Using Checkout Action
- **WHEN** a workflow step uses `actions/checkout`
- **THEN** it MUST use version `@v4`

#### Scenario: Using Artifact Upload Action
- **WHEN** a workflow step uses `actions/upload-artifact`
- **THEN** it MUST use version `@v4`
