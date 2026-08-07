## 2024-03-27 - [CRITICAL] Option Injection in test shell invocations
**Vulnerability:** Shell invocations using `popen` (e.g., in `tests/integration/display_integration_test.cpp`) sanitized arguments using `escapeShellArg` for shell quoting, but did not prevent option injection vulnerabilities where filenames starting with `-` are treated as flags by tools like ImageMagick.
**Learning:** `escapeShellArg` alone is not sufficient to prevent command option injection if input is directly passed to commands.
**Prevention:** Prepended `./` to relative paths via a `safeFilePath` helper function so they cannot be interpreted as command-line options.
## 2024-06-19 - Fix uint16_t underflow in display value clear
**Vulnerability:** Unsigned integer underflow in GUI rendering logic allowed potential out-of-bounds memory writes if text positions exceeded screen width.
**Learning:** When doing subtraction with bounds and unsigned integers, checking `a + b > MAX` is not enough if `a` can be `>= MAX`. This results in `MAX - a` underflowing to a massive integer, which when passed to drawing operations like `GUI_DrawRectangle`, wraps memory ranges.
**Prevention:** Always validate that the base coordinate itself is within the valid maximum boundary (e.g., `if (valX >= MAX) { width = 0; }`) before attempting to constrain the length.
## 2024-06-26 - [CRITICAL] Out-of-Bounds Memory Read in Display Text Rendering
**Vulnerability:** The `GUI_DisChar` function in `src/Display/LCD_GUI.cpp` calculated the font array offset without checking if the ASCII character provided was within the printable range (`' '` to `'~'`). Non-printable characters (like newlines or values > 127) caused an integer overflow/underflow, resulting in out-of-bounds memory reads from the `Font->table`.
**Learning:** Array index calculations derived directly from user input or external data (like characters in a string) must always be bounds-checked before accessing memory.
**Prevention:** Added a boundary check to map any out-of-bounds characters to a safe fallback character (e.g., `'?'`) before performing index math.

## 2026-03-01 - Add NULL pointer validation to LCD GUI
**Vulnerability:** Missing NULL pointer validation in embedded UI libraries causing potential Denial of Service (device crash).
**Learning:** Embedded systems are susceptible to crashes if pointers are not verified before dereferencing.
**Prevention:** Validate all pointers passed as arguments in public APIs before dereferencing them.
