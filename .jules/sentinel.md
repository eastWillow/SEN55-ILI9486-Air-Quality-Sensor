## 2024-05-20 - Option Injection Mitigation
**Vulnerability:** Shell option injection when passing user-controlled or variable file paths to shell commands (like ImageMagick's `compare` via `popen`). File paths starting with `-` are interpreted as command line options instead of files, even when quoted.
**Learning:** Simple string quoting (like `escapeShellArg`) is insufficient for preventing option injection if the argument is interpreted as a flag by the executable.
**Prevention:** Prepend `./` to relative file paths that start with `-` (or all file paths) to ensure the shell and the application interpret them as file paths, not as options. This pattern (e.g., `safeFilePath`) should be used in conjunction with standard quoting.
