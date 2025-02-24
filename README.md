# Logger Library for Embedded Systems such as Arduino

Logger provides simple logging with:

- `LOG`
- `LOGln` 
- `LOGI` (Info)
- `LOGW` (Warning)
- `LOGE` (Error)
- `LOGD` (Debug, only if `#def DEBUG`)
- `LOGIS`, `LOGIC`, `LOGIE`

functions.

In maing program you set: 
```
currentLogLevel = LOG_LEVEL_DEBUG;
```
or any of the following `LOG_LEVEL_INFO`, `LOG_LEVEL_WARN`, `LOG_LEVEL_ERROR`, `LOG_LEVEL_NONE`

and in then you can log with the `LOG` functions that act similar to `Serial.print();`

In your program if you set
```
#def DEBUG
```
LOGD will be compiled into the program and if its not set it will be omitted.

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
    LOGln("Data: %ld\n\r", data);
}

```
# Dependencies
None

# Contributing
ChatGpt

# License

See [LICENSE](LICENSE.md).
