## 1. Update Test Suite

- [x] 1.1 Update `NonBlockingDebounce` test case to account for 100ms feedback phase
- [x] 1.2 Update `ClockRollover` test case to handle deferred transitions and rollover logic
- [x] 1.3 Verify that `App_Loop` remains non-blocking (returns within expected time)

## 2. Verification

- [x] 2.1 Re-run `xvfb-run -a ctest -R "DebouncePerformanceTest" --output-on-failure --output-junit testReport-perf.xml`
- [x] 2.2 Verify `testReport-perf.xml` includes updated test cases
- [x] 2.3 Perform final check on all GUI-related unit tests to ensure no regressions
- [x] 2.4 Re-run `xvfb-run -a ctest -R "DisplayIntegrationTest" --output-on-failure --output-junit testReport-smoke.xml`
- [x] 2.5 Re-run `xvfb-run -a ctest -R "CoreLibTest" --output-on-failure --output-junit testReport-core.xml`
- [x] 2.6 Re-run `xvfb-run -a ctest -R "GUITest" --output-on-failure --output-junit testReport-gui.xml`
