## Context

The project uses Git submodules for dependencies. Currently, `.github/workflows/integration-test.yml` uses `actions/checkout@v4` without the `submodules: recursive` configuration, while other workflows like `.github/workflows/tests.yml` correctly include this configuration. This inconsistency means submodules won't be checked out during integration tests, potentially causing build failures.

## Goals / Non-Goals

**Goals:**
- Add `submodules: recursive` to the checkout step in `.github/workflows/integration-test.yml`
- Establish specifications for standardized GitHub Actions workflow configuration
- Ensure consistency across all workflow files

**Non-Goals:**
- Changing submodule dependencies or their structure
- Modifying other aspects of the workflow configurations beyond the checkout step
- Changing the version of `actions/checkout` being used (already on v4)

## Decisions

### Decision 1: Standardize checkout configuration across all workflows
**Rationale:** Using `submodules: recursive` ensures that all submodule dependencies are properly checked out. This should be consistent across all workflows to avoid subtle build failures.

**Alternatives considered:**
- Only fix integration-test.yml: Would solve the immediate issue but leave inconsistency risk for future workflows
- Use shallow submodule checkout: Rejected because full recursive checkout is already used in other workflows and ensures complete dependency resolution

### Decision 2: Document the standard in specifications
**Rationale:** Creating a specification ensures future workflows follow the same pattern and provides clear documentation for contributors.

**Alternatives considered:**
- Only document in a comment in the workflow files: Less discoverable and not part of the formal project specification system

## Risks / Trade-offs

**Risk:** Slightly longer checkout times in CI due to submodule cloning → **Mitigation:** This is already accepted in other workflows (tests.yml), and recursive checkout is necessary for correctness

**Risk:** Workflow breaks if submodules are removed in the future → **Mitigation:** This is a configuration issue to address when submodules are removed, not a reason to avoid correct configuration now
