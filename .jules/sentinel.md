## 2026-03-15 - Option Injection Prevention
**Vulnerability:** Option Injection in shell invocations.
**Learning:** Even with correctly escaped shell arguments, file paths starting with hyphens can be interpreted as flags by CLI tools, leading to unexpected behavior or arbitrary option execution.
**Prevention:** Always prepend `./` to relative file paths to explicitly indicate they are files, not options.
