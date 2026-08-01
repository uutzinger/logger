# Changelog

## v2.2.1 - 2026-08-01
- Added destination-explicit `logVPrintLevellnTo()` so a library can format to a supplied `Print` without reading or changing the global logger output.

## v2.2.0 - 2026-08-01
- Changed `LOG_LEVEL_*` macro values to match NimBLE/Mynewt (`DEBUG` = 0 through `NONE` = 5) to avoid macro redefinition warnings when both libraries are included.
- Inverted logger threshold comparisons so named log levels keep the same runtime behavior: `DEBUG` prints all enabled levels, `INFO` prints info/warn/error, `WARN` prints warn/error, `ERROR` prints error only, and `NONE` prints nothing.
- Added `LOG_LEVEL_CRITICAL` for compatibility with NimBLE/Mynewt level names.
- Migration note: code using raw numeric log levels or direct numeric comparisons should switch to the named `LOG_LEVEL_*` constants.

## v2.1.1 - 2026-07-31
- Added configurable `Print` output with `logSetOutput()` and `logGetOutput()`, while keeping `Serial` as the default.
- Added `logSetLevel()` and `logGetLevel()` as the preferred log-level API while preserving direct `currentLogLevel` compatibility.
- Added `logVPrint*()` variants for callers that already manage `va_list` arguments.
- Added optional `LOGGER_USE_SIMPLE_VSNPRINTF` support for builds that prefer the platform formatter over the custom `%b` implementation.
- Updated Arduino library metadata, keywords, and README examples for the new APIs and Arduino Library Manager compatibility.

## v2.0.0 - 2026-03-13
- Restored standard `printf` formatting while keeping `%b` as a custom 8-bit binary formatter.
- Replaced the duplicated formatter with one bounded implementation to avoid truncation-related memory corruption.
- Aligned `USE_AUDIO_LOGGING` so the library can coexist with the Arduino Audio logging macros.

## v1.0.0 - 2025-02-24
- First release.
