## 2024-03-27 - [CRITICAL] Option Injection in test shell invocations
**Vulnerability:** Shell invocations using `popen` (e.g., in `tests/integration/display_integration_test.cpp`) sanitized arguments using `escapeShellArg` for shell quoting, but did not prevent option injection vulnerabilities where filenames starting with `-` are treated as flags by tools like ImageMagick.
**Learning:** `escapeShellArg` alone is not sufficient to prevent command option injection if input is directly passed to commands.
**Prevention:** Prepended `./` to relative paths via a `safeFilePath` helper function so they cannot be interpreted as command-line options.
