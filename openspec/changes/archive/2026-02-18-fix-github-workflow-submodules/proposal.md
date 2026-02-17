## Why

The `.github/workflows/integration-test.yml` workflow uses `actions/checkout@v4` but is missing the `submodules: recursive` configuration. This inconsistency with other workflows (like `tests.yml`) means submodules won't be checked out during integration tests, potentially causing build failures if the project depends on submodules.

## What Changes

- Add `submodules: recursive` configuration to the `actions/checkout@v4` step in `.github/workflows/integration-test.yml`
- Ensure consistency across all GitHub Actions workflows in the repository
- Update the project specifications to document the GitHub workflow configuration requirements

## Capabilities

### New Capabilities
- `github-workflow-configuration`: Standardized configuration for GitHub Actions workflows, specifically requiring `actions/checkout@v4` with `submodules: recursive` for all workflows

### Modified Capabilities
<!-- No existing capabilities require requirement changes - this is a new specification -->

## Impact

- **Affected Files:** `.github/workflows/integration-test.yml`
- **CI/CD:** Integration test workflow will now properly check out submodules, ensuring complete project checkout
- **Consistency:** All workflows will follow the same checkout pattern
- **Specifications:** New spec document will establish the standard for GitHub Actions workflow configuration
