# Logger Library for Embedded Systems such as Arduino

Logger provides simple logging (`Print` output macros, defaulting to `Serial`) with:

- `LOG`
- `LOGln` 
- `LOGI` (Info)
- `LOGW` (Warning)
- `LOGE` (Error)
- `LOGD` (Debug, only if `#def DEBUG`)
- `LOGIS`, `LOGIC`, `LOGIE`

functions.

The formatter supports standard `printf` specifiers such as `%d`, `%ld`, `%u`, `%x`, `%f`, `%c`, `%s`, and `%p`.
The custom `%b` specifier prints the low 8 bits of the supplied value as a binary string.

In your main program you set the level with: 
```
logSetLevel(LOG_LEVEL_DEBUG);
```
or any of the following `LOG_LEVEL_INFO`, `LOG_LEVEL_WARN`, `LOG_LEVEL_ERROR`, `LOG_LEVEL_NONE`
and then you can log with the `LOG` functions. 

If the current level is `INFO`, `WARNING` and `ERROR` messages will also be printed. If it is level `NONE`, no logging will occur.
The `LOG_LEVEL_*` macro values match NimBLE/Mynewt (`DEBUG` is `0`, `NONE` is `5`) to avoid redefinition warnings when both libraries are used. Use the named constants rather than raw numbers.

`LOG` and `LOGln` omit `[LOG LEVEL]` in the output, and it will occur regardless of the current log level.

Existing sketches that assign `currentLogLevel` directly continue to work, but `logSetLevel()` is the preferred API for new code.

By default logs are written to `Serial`. To write to another Arduino `Print` implementation, such as BLESerial, set the output once:
```
logSetOutput(bleSerial);
LOGI("Logging over BLE");
```

Libraries that already manage a `va_list` can format a leveled line directly to
any `Print`, without reading or changing the global output, using
`logVPrintLevellnTo()`.

In your program if you set
```
#define DEBUG
```
LOGD will be compiled into the program and if its not set it will be omitted. This will reduce the overhead once only INFO, WARNING or ERROR logging is wanted.

If you use the `Arduino Audio library` and want to avoid conflicts with its `LOGE`, `LOGI`, `LOGW`, and related macros, define
```
#define USE_AUDIO_LOGGING
```
before including `logger.h`. In that mode `LOG`, `LOGln`, `logPrint`, and `logPrintln` remain available, but the level macros, level constants, `currentLogLevel`, `logSetLevel()`, and `logGetLevel()` are not declared by this library.

# Implementation
The library uses compact macros as log-level gates. This avoids formatting the log text when the log level is not met.

For portability, it implements its own `printf`-style formatting and includes a `%b` formatter. If code size is more important than `%b` support, define
```
#define LOGGER_USE_SIMPLE_VSNPRINTF
```
before including `logger.h` to use the platform `vsnprintf()` formatter directly.

# Installation
url=https://github.com/uutzinger/logger
or use the Arduino library manager.

# Example Use
```
/////////////////////////////////////////////////////////////////////////////////////////
// Example Program using Logger
/////////////////////////////////////////////////////////////////////////////////////////

#include <Arduino.h>
#include <logger.h>

/////////////////////////////////////////////////////////////////////////////////////////
// Select the  default log level, you can change this at runtime also
// #define LOG_LEVEL LOG_LEVEL_NONE 
// #define LOG_LEVEL LOG_LEVEL_ERROR
// #define LOG_LEVEL LOG_LEVEL_WARN 
#define LOG_LEVEL LOG_LEVEL_INFO 
// #define LOG_LEVEL LOG_LEVEL_DEBUG
//
#define BAUDRATE 500000 // Up to 2,000,000 on ESP32, however more than 500 kBaud might be unreliable
/////////////////////////////////////////////////////////////////////////////////////////

...

void setup()
{
  logSetLevel(LOG_LEVEL);

  Serial.begin(BAUDRATE);
  while ( !Serial ){ delay(100); }

  // Optional: route logs to any Print target instead of Serial.
  // logSetOutput(bleSerial);

  LOGI("mySensor");
  ...

  LOGI("Reading AFE Info");
  while (!mySensor.readInfo()){
    LOGE("Sensor: failed to read ID, please make sure all the pins are connected");
    delay(1000);
  }

  LOGI("Initializing  ...");
  mySensor.begin();
}

void loop() {
    data = mySensor.get ...;    
    LOGln("Data: %ld", data);
}

```
# Dependencies
None

# Contributing
OpenAI Codex

# License

See [LICENSE](LICENSE.md).
