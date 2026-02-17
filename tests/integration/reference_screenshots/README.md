# Integration Test Reference Screenshots

This directory contains the golden "known good" screenshots for visual regression testing.

## Update Process

When you make intentional UI changes, the integration tests will fail. To update the references:

1.  Run the tests locally:
    ```bash
    cd build
    ./tests/display_integration_test
    ```
2.  Inspect the `actual_*.bmp` files generated in the build directory.
3.  If they look correct, copy them to this directory:
    ```bash
    cp build/actual_startup.bmp tests/integration/reference_screenshots/startup.bmp
    cp build/actual_final.bmp tests/integration/reference_screenshots/final.bmp
    ```
4.  Commit the changes.

## Files

- `startup.bmp`: Screenshot at Frame 0 (after `App_Setup`)
- `final.bmp`: Screenshot at Frame 25 (after full render cycle)
