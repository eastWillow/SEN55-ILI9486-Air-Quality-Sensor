#ifndef APP_LOG_H
#define APP_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

// Logs a formatted message followed by a newline.
// Behaves like printf but adds a newline automatically.
void App_Log(const char* format, ...);

#ifdef __cplusplus
}
#endif

#endif // APP_LOG_H
