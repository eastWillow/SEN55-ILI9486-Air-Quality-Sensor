## 1. Clean Up Existing Workflows

- [x] 1.1 Delete `.github/workflows/integration-test.yml` as it will be merged into `tests.yml`.

## 2. Unify the Test Workflow

- [x] 2.1 Modify `.github/workflows/tests.yml` to include a single project build step instead of multiple separate builds.
- [x] 2.2 Configure the job to run tests in sequential steps rather than a single `ctest` command.

## 3. Implement Sequential Test Steps

- [x] 3.1 Add **Step 1: Check Smoke Test**. Configure it so a failure halts the workflow immediately (`if: failure()` or default behavior) before running subsequent tests.
- [x] 3.2 Add **Step 2: Check CoreLibTest** to run after Smoke Test passes.
- [x] 3.3 Add **Step 3: Check GUITest** to run after CoreLibTest passes.
- [x] 3.4 Add **Step 4: Check PerformanceTest** to run after GUITest passes.

## 4. Test Reporting and Artifacts

- [x] 4.1 Update the CTest commands in all test steps to generate XML test reports (e.g., using `--output-junit`).
- [x] 4.2 Aggregate the separate test reports into a single `testReport.xml` logically, or configure `dorny/test-reporter` to pick them up cohesively.
- [x] 4.3 Add a step to upload the `testReport.xml` and any generated artifacts (e.g., screenshots from Smoke or GUI tests). Ensure this runs even if earlier test steps fail by using `if: always()` or `if: !cancelled()`.
