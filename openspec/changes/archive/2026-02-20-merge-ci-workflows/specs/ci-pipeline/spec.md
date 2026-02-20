## MODIFIED Requirements

### Requirement: Independent Test Pipeline
The CI/CD test workflow must be able to build tests efficiently using a unified configuration, removing redundant builds across workflows while maintaining standalone execution capabilities.

#### Scenario: Running Tests in CI
- **WHEN** The unified tests workflow is triggered in GitHub Actions
- **THEN** The workflow configures CMake directly from the `tests` subdirectory (or root, whichever is optimal)
- **AND** The build completes successfully exactly once without requiring multiple builds for different test stages
- **AND** All tests execute and pass sequentially in their respective steps (Smoke, CoreLib, GUI, Performance)
