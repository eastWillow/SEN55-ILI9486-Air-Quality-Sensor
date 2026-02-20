## Why

The current CI pipeline has redundant build processes across `integration-test.yml` and `tests.yml`. Merging these workflows reduces build duplication, speeds up the CI pipeline, and provides a unified point of entry for test execution. This ensures clearer test results (one `testReport.xml`) and tighter control over failure conditions, like stopping early if the smoke tests fail.

## What Changes

- Merge `integration-test.yml` and `tests.yml` into a single action file.
- Unify the test reporting into a single `testReport.xml`.
- Remove redundant/duplicate build processes.
- Restructure jobs into sequential steps within the unified workflow:
  - Step 1: Check Smoke Test (if failed, end immediately and upload artifacts).
  - Step 2: Check CoreLibTest.
  - Step 3: Check GUITest.
  - Step 4: Check PerformanceTest.

## Capabilities

### New Capabilities

- `unified-ci-pipeline`: This replaces the split CI definitions with a unified pipeline that has distinct steps (Smoke, CoreLib, GUI, Performance) and fail-fast behavior.

### Modified Capabilities

- `ci-pipeline`: Consolidating workflows and restructuring into steps changes the way the system is tested and reports results.

## Impact

- Github Actions Workflows (`.github/workflows/integration-test.yml`, `.github/workflows/tests.yml` will be merged/removed/replaced).
- Build processes triggered by PRs will be faster due to the consolidated build step.
- The `testReport.xml` generation will aggregate results from all test types.
