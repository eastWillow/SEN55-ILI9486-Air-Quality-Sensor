## Purpose

The purpose of this capability is to provide a single, unified CI/CD workflow that handles all forms of testing, structured as sequential steps, ensuring fail-fast execution and centralized reporting.

## Requirements

### Requirement: Unified Test Workflow
The CI/CD pipeline MUST execute all test categories (Smoke, CoreLib, GUI, Performance) in a single workflow file to avoid redundant build steps.

#### Scenario: Running the merged workflow
- **WHEN** the unified test workflow is triggered
- **THEN** it builds the project exactly once
- **AND** it runs the tests sequentially as distinct steps

### Requirement: Fail-Fast Smoke Tests
The unified workflow MUST terminate early if the Smoke Test step fails, preventing execution of subsequent tests.

#### Scenario: Smoke Test Failure
- **WHEN** the Smoke Test step fails
- **THEN** the workflow execution is halted immediately
- **AND** any generated test artifacts or screenshots are uploaded

### Requirement: Unified Test Report
The test workflow MUST output or consolidate results into a single `testReport.xml` artifact.

#### Scenario: Generating the unified report
- **WHEN** all steps complete (or fail after Smoke tests)
- **THEN** a `testReport.xml` containing results from the executed test categories is generated and uploaded
