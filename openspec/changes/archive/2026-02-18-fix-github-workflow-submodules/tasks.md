## 1. Update Integration Test Workflow

- [x] 1.1 Add `submodules: recursive` configuration to the checkout step in `.github/workflows/integration-test.yml`
- [x] 1.2 Verify the checkout step follows the same pattern as `.github/workflows/tests.yml`

## 2. Verify Consistency

- [x] 2.1 Review all other workflow files (`.github/workflows/*.yml`) to ensure they use consistent checkout configuration
- [x] 2.2 Document the standard checkout pattern if not already documented
