#include "Log.h"
#include <stdio.h>
#include <stdarg.h>

#ifdef ARDUINO
#include <Arduino.h>
#endif

void App_Log(const char* format, ...) {
    va_list args;
    va_start(args, format);
#ifdef ARDUINO
    char buffer[512];
    vsnprintf(buffer, sizeof(buffer), format, args);
    Serial.println(buffer);
#else
    vprintf(format, args);
    printf("\n");
#endif
    va_end(args);
}
