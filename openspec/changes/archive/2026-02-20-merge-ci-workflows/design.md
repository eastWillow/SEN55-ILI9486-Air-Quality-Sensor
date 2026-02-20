## Context

The current status of continuous integration uses two different files, `integration-test.yml` and `tests.yml`, which results in redundant build steps and disjointed testing feedback. Having disjointed test artifacts (`testReport.xml`) makes it harder to parse results effectively.

## Goals / Non-Goals

**Goals:**
- Combine the builds for integration and unit tests to happen exactly once.
- Break testing into discrete, dependent steps: Smoke Test -> CoreLibTest -> GUITest -> PerformanceTest.
- Fail fast: If the Smoke Test step fails, fail the whole pipeline and upload artifacts immediately.
- Aggregate all CTest results into a single `testReport.xml`.
- Delete the redundant workflow.

**Non-Goals:**
- We are not changing the source files of the code itself, only tweaking the CMake/CTest invocations slightly if needed, but primarily focusing on GitHub Actions configurations.
- We are not adding new types of tests beyond what's already defined (Smoke, Core, GUI, Performance).

## Decisions

- **Workflow File:** We will retain one workflow file (e.g., `tests.yml`) and delete `integration-test.yml`.
- **Jobs vs Steps:** Instead of separate jobs for building and different testing environments which duplicate the build phase, we will have a single job `build-and-test`. Inside it, we compile the project once. Then we run tests sequentially as steps.
- **Fail-Fast with Artifacts:** We will configure the "Smoke Test" step so that if it fails, GitHub Actions naturally stops following steps. Then an `always()` or `failure()` conditioned step will upload the `testReport.xml` and any screenshots.
- **Reporting:** CTest allows outputting Junit XML. We'll specify `--output-junit testReport.xml` on each test run, appending or combining as needed. Actually, CTest overwrites by default. We might need to output them to distinct XML files and parse them, OR run a single CTest invocation with labels. A single CTest invocation with labels is best, but sequential fail-fast requires separate steps. Therefore we can either run separate CTest commands and upload the directory, or combine them using a script. A simpler approach is to output `testReport-smoke.xml`, `testReport-core.xml`, etc., but the user explicitly requested "one testReport.xml". We can achieve this by using `pytest --junitxml=testReport.xml` or using `ctest` with a single command if possible, BUT we need separate steps. If they are separate steps, we can append to the XML or have a tool merge them, OR we can configure the `dorny/test-reporter` to pick up `testReport*.xml` via glob, presenting it as one report. The instruction says "one testReport.xml".

## Risks / Trade-offs

- **Risk:** Timeouts. All steps happening sequentially in one job could be slower if tests were previously parallelized across jobs.
  - **Mitigation:** Removing the redundant build step saves significant time, likely offsetting the sequential test execution time.
- **Risk:** CTest JUnit support overwriting the report.
  - **Mitigation:** We'll name them `testReport-smoke.xml`, `testReport-core.xml`, etc., and merge them or rely on the reporter action accepting a glob and merging it logically.
