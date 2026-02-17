## ADDED Requirements

### Requirement: GitHub Actions workflows MUST use checkout with recursive submodules
All GitHub Actions workflows that build or test the project SHALL use `actions/checkout@v4` with `submodules: recursive` configuration to ensure complete repository checkout including all submodule dependencies.

#### Scenario: Workflow checks out repository with submodules
- **WHEN** a GitHub Actions workflow runs that requires the project code
- **THEN** the workflow MUST include a checkout step with `uses: actions/checkout@v4` and `with: submodules: recursive`

#### Scenario: Integration test workflow checks out submodules
- **WHEN** the integration test workflow (`.github/workflows/integration-test.yml`) runs
- **THEN** the workflow MUST check out all submodules recursively before building or testing

#### Scenario: Unit test workflow checks out submodules
- **WHEN** the unit test workflow (`.github/workflows/tests.yml`) runs
- **THEN** the workflow MUST check out all submodules recursively before building or testing

### Requirement: Consistency across workflow files
All workflow files that require source code SHALL use the same checkout configuration to prevent build failures due to missing dependencies.

#### Scenario: Multiple workflows use consistent checkout
- **WHEN** comparing checkout steps across different workflow files
- **THEN** all workflows that build or test the project MUST use identical checkout configuration including submodule settings

#### Scenario: New workflow follows standard pattern
- **WHEN** creating a new GitHub Actions workflow that requires source code
- **THEN** the workflow MUST follow the established checkout pattern with recursive submodules
