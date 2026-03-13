# Changelog
2026 Mar 13
- Restored standard `printf` formatting while keeping `%b` as a custom 8-bit binary formatter.
- Replaced the duplicated formatter with one bounded implementation to avoid truncation-related memory corruption.
- Aligned `USE_AUDIO_LOGGING` so the library can coexist with the Arduino Audio logging macros.

2025 Feb 24 First release.
