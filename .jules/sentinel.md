## 2024-03-27 - [CRITICAL] Option Injection in test shell invocations
**Vulnerability:** Shell invocations using `popen` (e.g., in `tests/integration/display_integration_test.cpp`) sanitized arguments using `escapeShellArg` for shell quoting, but did not prevent option injection vulnerabilities where filenames starting with `-` are treated as flags by tools like ImageMagick.
**Learning:** `escapeShellArg` alone is not sufficient to prevent command option injection if input is directly passed to commands.
**Prevention:** Prepended `./` to relative paths via a `safeFilePath` helper function so they cannot be interpreted as command-line options.
## 2024-06-19 - Fix uint16_t underflow in display value clear
**Vulnerability:** Unsigned integer underflow in GUI rendering logic allowed potential out-of-bounds memory writes if text positions exceeded screen width.
**Learning:** When doing subtraction with bounds and unsigned integers, checking `a + b > MAX` is not enough if `a` can be `>= MAX`. This results in `MAX - a` underflowing to a massive integer, which when passed to drawing operations like `GUI_DrawRectangle`, wraps memory ranges.
**Prevention:** Always validate that the base coordinate itself is within the valid maximum boundary (e.g., `if (valX >= MAX) { width = 0; }`) before attempting to constrain the length.
