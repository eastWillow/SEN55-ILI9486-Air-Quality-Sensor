
## 2024-10-25 - Format String Vulnerability in Debug Macro
**Vulnerability:** Found a format string vulnerability in `src/Display/Debug.h` where `DEBUG(__info)` was defined as `printf(__info)` on PC builds. This allowed format specifiers in the input string to be interpreted by `printf`, potentially leading to stack reads or memory corruption.
**Learning:** Legacy or embedded C code often wraps `printf` in macros without considering format string safety. In C++, using `iostream` provides a type-safe alternative that avoids this class of vulnerabilities entirely.
**Prevention:** Use `std::cout` (or other stream-based logging) instead of `printf` for logging macros in C++, or ensure `printf` is called with a fixed format string like `printf("%s", __info)`.
