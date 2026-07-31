# Changelog

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
