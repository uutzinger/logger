#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include <stdarg.h>

#ifndef USE_AUDIO_LOGGING

// Current log level. Prefer logSetLevel() in new sketches.
extern int currentLogLevel;

// Log levels
#define LOG_LEVEL_NONE  0
#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_WARN  2
#define LOG_LEVEL_INFO  3
#define LOG_LEVEL_DEBUG 4

// Log level configuration. Defaults to DEBUG.
void logSetLevel(int level);
int logGetLevel();

// Macros for logging
#define LOGE(...) do { if (currentLogLevel >= LOG_LEVEL_ERROR) logPrintLevelln("ERROR", __VA_ARGS__); } while (0)
#define LOGW(...) do { if (currentLogLevel >= LOG_LEVEL_WARN)  logPrintLevelln("WARN",  __VA_ARGS__); } while (0)
#define LOGI(...) do { if (currentLogLevel >= LOG_LEVEL_INFO)  logPrintLevelln("INFO",  __VA_ARGS__); } while (0)

#ifdef DEBUG
    #define LOGD(...) do { if (currentLogLevel >= LOG_LEVEL_DEBUG) logPrintLevelln("DEBUG", __VA_ARGS__); } while (0)
#else
    #define LOGD(...) do { } while (0)
#endif

// Continuous info logging on the same line with multiple calls
#define LOGIS(...) do { if (currentLogLevel >= LOG_LEVEL_INFO) logPrintLevel("INFO",  __VA_ARGS__); } while (0)
#define LOGIC(...) do { if (currentLogLevel >= LOG_LEVEL_INFO) logPrint(__VA_ARGS__); } while (0)
#define LOGIE(...) do { if (currentLogLevel >= LOG_LEVEL_INFO) logPrintln(__VA_ARGS__); } while (0)

#endif // USE_AUDIO_LOGGING

// Simple log macros
#define LOG(...) logPrint(__VA_ARGS__)
#define LOGln(...) logPrintln(__VA_ARGS__)

// Output stream configuration. Defaults to Serial.
void logSetOutput(Print& output);
Print& logGetOutput();

// Functions to print log messages
void logPrintLevel(const char* level, const char* format, ...);
void logPrintLevelln(const char* level, const char* format, ...);
void logPrint(const char* format, ...);
void logPrintln(const char* format, ...);
void logVPrintLevel(const char* level, const char* format, va_list args);
void logVPrintLevelln(const char* level, const char* format, va_list args);
void logVPrint(const char* format, va_list args);
void logVPrintln(const char* format, va_list args);

// Binary conversion function
void uint8ToBinaryString(char *buffer, uint8_t value);

#endif // LOGGER_H
