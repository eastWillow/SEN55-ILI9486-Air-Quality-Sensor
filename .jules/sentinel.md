
## 2026-03-19 - [HIGH] Fix command option injection in tests
**Vulnerability:** In `tests/integration/display_integration_test.cpp`, the arguments to `popen` were escaped using `escapeShellArg` to prevent command injection, but they were not protected against option injection. If an image file name started with `-` (e.g., `-metric.bmp`), the `compare` tool might interpret it as a flag instead of a file name.
**Learning:** Shell argument escaping prevents shell metadata injection, but does not prevent the target binary from misinterpreting a file name as a flag. This is a common form of command option injection.
**Prevention:** To prevent option injection, prepend `./` to relative paths when passing user-controlled file paths to external commands.
