## 2024-05-24 - Prevent Option Injection in Shell Commands
**Vulnerability:** Option injection vulnerability in `popen` shell invocations. Filenames starting with `-` are treated as command-line flags by tools like ImageMagick, even if they are properly shell-escaped/quoted.
**Learning:** Shell quoting prevents command injection (e.g., executing arbitrary shell commands) but does not prevent option injection for the executed program.
**Prevention:** Always prepend `./` to relative file paths passed to shell commands to ensure they are treated as positional arguments and not flags.
