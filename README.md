# Logger Library for Embedded Systems such as Arduino

Logger provides simple logging (Serial.print MACROS) with:

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

In your main program you set: 
```
currentLogLevel = LOG_LEVEL_DEBUG;
```
or any of the following `LOG_LEVEL_INFO`, `LOG_LEVEL_WARN`, `LOG_LEVEL_ERROR`, `LOG_LEVEL_NONE`

and then you can log with the `LOG` functions. If currentLogLevel is `INFO`, `WARNING` and `ERROR` messages will also be printed. If it is level `NONE`, no logging will occur.
`LOG` and `LOGln` work similarly to simple printing, but you can use format strings like `LOGln("Data: %ld", data);`. Output will occur regardless of `currentLogLevel` for `LOG` and `LOGln`.

In your program if you set
```
#define DEBUG
```
LOGD will be compiled into the program and if its not set it will be omitted. This will reduce the overhead once only INFO, WARNING or ERROR level is wanted.

If you use the Arduino Audio library and want to avoid conflicts with its `LOGE`, `LOGI`, `LOGW`, and related macros, define
```
#define USE_AUDIO_LOGGING
```
before including `logger.h`. In that mode `LOG`, `LOGln`, `logPrint`, and `logPrintln` remain available, but the level macros and `currentLogLevel` are not declared by this library.

# Installation
url=https://github.com/uutzinger/logger

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
  currentLogLevel = LOG_LEVEL;

  Serial.begin(BAUDRATE);
  while ( !Serial ){ delay(100); }

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
Gpt-5.4

# License

See [LICENSE](LICENSE.md).
