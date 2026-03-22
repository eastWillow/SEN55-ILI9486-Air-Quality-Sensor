## 2024-05-09 - Prevent Option Injection in Shell Commands
**Vulnerability:** Shell invocations using `popen` correctly escaped arguments, but failed to prevent option injection. If a filename supplied to `popen` started with a `-`, the invoked command (e.g., ImageMagick's `compare`) would treat it as an option instead of a file.
**Learning:** Argument escaping is not enough to prevent injection. Option injection allows an attacker to manipulate the execution flow of the command by supplying arguments that look like options.
**Prevention:** Prepend `./` to all relative file paths used in shell commands before escaping them. The `safeFilePath` helper accomplishes this to ensure the path is unambiguously treated as a file, preventing the tool from parsing it as an option flag.
