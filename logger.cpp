#include "logger.h"

#include <limits.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#if SIZE_MAX == UINT_MAX
typedef int logger_signed_size_t;
#elif SIZE_MAX == ULONG_MAX
typedef long logger_signed_size_t;
#elif defined(ULLONG_MAX) && (SIZE_MAX == ULLONG_MAX)
typedef long long logger_signed_size_t;
#else
typedef ptrdiff_t logger_signed_size_t;
#endif

namespace {

static const size_t LOGGER_BUFFER_SIZE = 256;
static const size_t LOGGER_FORMAT_SIZE = 64;
static const size_t LOGGER_CHUNK_SIZE = LOGGER_BUFFER_SIZE;

enum LengthModifier {
    LENGTH_NONE,
    LENGTH_HH,
    LENGTH_H,
    LENGTH_L,
    LENGTH_LL,
    LENGTH_J,
    LENGTH_Z,
    LENGTH_T,
    LENGTH_CAPITAL_L
};

struct FormatSpecifier {
    const char* begin;
    const char* end;
    bool widthFromArg;
    bool precisionFromArg;
    LengthModifier length;
    char specifier;
};

static bool isDigitChar(char value) {
    return (value >= '0') && (value <= '9');
}

static void appendData(char* buffer, size_t bufferSize, size_t& index, const char* data, size_t length) {
    if ((bufferSize == 0) || (length == 0)) {
        return;
    }

    size_t available = 0;
    if (index < (bufferSize - 1)) {
        available = (bufferSize - 1) - index;
    }

    size_t copyLength = length;
    if (copyLength > available) {
        copyLength = available;
    }

    if (copyLength > 0) {
        memcpy(buffer + index, data, copyLength);
        index += copyLength;
    }

    buffer[index] = '\0';
}

static void appendChar(char* buffer, size_t bufferSize, size_t& index, char value) {
    appendData(buffer, bufferSize, index, &value, 1);
}

static bool parseFormatSpecifier(const char* format, FormatSpecifier& spec) {
    if ((format == NULL) || (*format != '%')) {
        return false;
    }

    const char* cursor = format + 1;
    spec.begin = format;
    spec.widthFromArg = false;
    spec.precisionFromArg = false;
    spec.length = LENGTH_NONE;
    spec.specifier = '\0';
    spec.end = format;

    while ((*cursor == '-') || (*cursor == '+') || (*cursor == ' ') || (*cursor == '#') || (*cursor == '0')) {
        ++cursor;
    }

    if (*cursor == '*') {
        spec.widthFromArg = true;
        ++cursor;
    } else {
        while (isDigitChar(*cursor)) {
            ++cursor;
        }
    }

    if (*cursor == '.') {
        ++cursor;
        if (*cursor == '*') {
            spec.precisionFromArg = true;
            ++cursor;
        } else {
            while (isDigitChar(*cursor)) {
                ++cursor;
            }
        }
    }

    if ((cursor[0] == 'h') && (cursor[1] == 'h')) {
        spec.length = LENGTH_HH;
        cursor += 2;
    } else if (*cursor == 'h') {
        spec.length = LENGTH_H;
        ++cursor;
    } else if ((cursor[0] == 'l') && (cursor[1] == 'l')) {
        spec.length = LENGTH_LL;
        cursor += 2;
    } else if (*cursor == 'l') {
        spec.length = LENGTH_L;
        ++cursor;
    } else if (*cursor == 'j') {
        spec.length = LENGTH_J;
        ++cursor;
    } else if (*cursor == 'z') {
        spec.length = LENGTH_Z;
        ++cursor;
    } else if (*cursor == 't') {
        spec.length = LENGTH_T;
        ++cursor;
    } else if (*cursor == 'L') {
        spec.length = LENGTH_CAPITAL_L;
        ++cursor;
    }

    if (*cursor == '\0') {
        return false;
    }

    spec.specifier = *cursor;
    spec.end = cursor + 1;
    return true;
}

static bool copySpecifierFormat(char* destination, size_t destinationSize, const FormatSpecifier& spec, char replacementSpecifier) {
    if (destinationSize == 0) {
        return false;
    }

    size_t length = static_cast<size_t>(spec.end - spec.begin);
    if ((length == 0) || (length >= destinationSize)) {
        destination[0] = '\0';
        return false;
    }

    memcpy(destination, spec.begin, length);
    destination[length - 1] = replacementSpecifier;
    destination[length] = '\0';
    return true;
}

template <typename T>
static int formatValue(char* buffer, size_t bufferSize, const char* format, bool widthFromArg, bool precisionFromArg, int width, int precision, T value) {
    if (widthFromArg && precisionFromArg) {
        return snprintf(buffer, bufferSize, format, width, precision, value);
    }
    if (widthFromArg) {
        return snprintf(buffer, bufferSize, format, width, value);
    }
    if (precisionFromArg) {
        return snprintf(buffer, bufferSize, format, precision, value);
    }
    return snprintf(buffer, bufferSize, format, value);
}

static void storeCountArgument(va_list& args, const FormatSpecifier& spec, size_t count) {
    switch (spec.length) {
        case LENGTH_HH:
            *va_arg(args, signed char*) = static_cast<signed char>(count);
            break;
        case LENGTH_H:
            *va_arg(args, short*) = static_cast<short>(count);
            break;
        case LENGTH_L:
            *va_arg(args, long*) = static_cast<long>(count);
            break;
        case LENGTH_LL:
            *va_arg(args, long long*) = static_cast<long long>(count);
            break;
        case LENGTH_J:
            *va_arg(args, intmax_t*) = static_cast<intmax_t>(count);
            break;
        case LENGTH_Z:
            *va_arg(args, logger_signed_size_t*) = static_cast<logger_signed_size_t>(count);
            break;
        case LENGTH_T:
            *va_arg(args, ptrdiff_t*) = static_cast<ptrdiff_t>(count);
            break;
        case LENGTH_NONE:
        case LENGTH_CAPITAL_L:
        default:
            *va_arg(args, int*) = static_cast<int>(count);
            break;
    }
}

static int formatSpecifierValue(char* chunk, size_t chunkSize, const FormatSpecifier& spec, va_list& args, size_t currentCount) {
    int width = 0;
    int precision = 0;

    if (spec.widthFromArg) {
        width = va_arg(args, int);
    }
    if (spec.precisionFromArg) {
        precision = va_arg(args, int);
    }

    if (spec.specifier == 'n') {
        storeCountArgument(args, spec, currentCount);
        if (chunkSize > 0) {
            chunk[0] = '\0';
        }
        return 0;
    }

    char formatBuffer[LOGGER_FORMAT_SIZE];
    if (!copySpecifierFormat(formatBuffer, sizeof(formatBuffer), spec, spec.specifier)) {
        return -1;
    }

    switch (spec.specifier) {
        case 'd':
        case 'i':
            switch (spec.length) {
                case LENGTH_HH:
                case LENGTH_H:
                case LENGTH_NONE:
                    return formatValue(chunk, chunkSize, formatBuffer, spec.widthFromArg, spec.precisionFromArg, width, precision, va_arg(args, int));
                case LENGTH_L:
                    return formatValue(chunk, chunkSize, formatBuffer, spec.widthFromArg, spec.precisionFromArg, width, precision, va_arg(args, long));
                case LENGTH_LL:
                    return formatValue(chunk, chunkSize, formatBuffer, spec.widthFromArg, spec.precisionFromArg, width, precision, va_arg(args, long long));
                case LENGTH_J:
                    return formatValue(chunk, chunkSize, formatBuffer, spec.widthFromArg, spec.precisionFromArg, width, precision, va_arg(args, intmax_t));
                case LENGTH_Z:
                    return formatValue(chunk, chunkSize, formatBuffer, spec.widthFromArg, spec.precisionFromArg, width, precision, va_arg(args, logger_signed_size_t));
                case LENGTH_T:
                    return formatValue(chunk, chunkSize, formatBuffer, spec.widthFromArg, spec.precisionFromArg, width, precision, va_arg(args, ptrdiff_t));
                default:
                    return -1;
            }

        case 'u':
        case 'o':
        case 'x':
        case 'X':
            switch (spec.length) {
                case LENGTH_HH:
                #if UCHAR_MAX <= INT_MAX
                    return formatValue(chunk, chunkSize, formatBuffer, spec.widthFromArg, spec.precisionFromArg, width, precision, va_arg(args, int));
                #else
                    return formatValue(chunk, chunkSize, formatBuffer, spec.widthFromArg, spec.precisionFromArg, width, precision, va_arg(args, unsigned int));
                #endif
                case LENGTH_H:
                #if USHRT_MAX <= INT_MAX
                    return formatValue(chunk, chunkSize, formatBuffer, spec.widthFromArg, spec.precisionFromArg, width, precision, va_arg(args, int));
                #else
                    return formatValue(chunk, chunkSize, formatBuffer, spec.widthFromArg, spec.precisionFromArg, width, precision, va_arg(args, unsigned int));
                #endif
                case LENGTH_NONE:
                    return formatValue(chunk, chunkSize, formatBuffer, spec.widthFromArg, spec.precisionFromArg, width, precision, va_arg(args, unsigned int));
                case LENGTH_L:
                    return formatValue(chunk, chunkSize, formatBuffer, spec.widthFromArg, spec.precisionFromArg, width, precision, va_arg(args, unsigned long));
                case LENGTH_LL:
                    return formatValue(chunk, chunkSize, formatBuffer, spec.widthFromArg, spec.precisionFromArg, width, precision, va_arg(args, unsigned long long));
                case LENGTH_J:
                    return formatValue(chunk, chunkSize, formatBuffer, spec.widthFromArg, spec.precisionFromArg, width, precision, va_arg(args, uintmax_t));
                case LENGTH_Z:
                    return formatValue(chunk, chunkSize, formatBuffer, spec.widthFromArg, spec.precisionFromArg, width, precision, va_arg(args, size_t));
                case LENGTH_T:
                    return formatValue(chunk, chunkSize, formatBuffer, spec.widthFromArg, spec.precisionFromArg, width, precision, static_cast<size_t>(va_arg(args, ptrdiff_t)));
                default:
                    return -1;
            }

        case 'f':
        case 'F':
        case 'e':
        case 'E':
        case 'g':
        case 'G':
        case 'a':
        case 'A':
            if (spec.length == LENGTH_CAPITAL_L) {
                return formatValue(chunk, chunkSize, formatBuffer, spec.widthFromArg, spec.precisionFromArg, width, precision, va_arg(args, long double));
            }
            return formatValue(chunk, chunkSize, formatBuffer, spec.widthFromArg, spec.precisionFromArg, width, precision, va_arg(args, double));

        case 'c':
            return formatValue(chunk, chunkSize, formatBuffer, spec.widthFromArg, spec.precisionFromArg, width, precision, va_arg(args, int));

        case 's': {
            const char* value = va_arg(args, const char*);
            if (value == NULL) {
                value = "(null)";
            }
            return formatValue(chunk, chunkSize, formatBuffer, spec.widthFromArg, spec.precisionFromArg, width, precision, value);
        }

        case 'p':
            return formatValue(chunk, chunkSize, formatBuffer, spec.widthFromArg, spec.precisionFromArg, width, precision, va_arg(args, void*));

        case 'b': {
            char binary[9];
            char binaryFormat[LOGGER_FORMAT_SIZE];
            if (!copySpecifierFormat(binaryFormat, sizeof(binaryFormat), spec, 's')) {
                return -1;
            }
            uint8ToBinaryString(binary, static_cast<uint8_t>(va_arg(args, int)));
            return formatValue(chunk, chunkSize, binaryFormat, spec.widthFromArg, spec.precisionFromArg, width, precision, binary);
        }

        default:
            return -1;
    }
}

static void formatLogMessage(char* buffer, size_t bufferSize, const char* format, va_list& args) {
    if (bufferSize == 0) {
        return;
    }

    buffer[0] = '\0';
    if (format == NULL) {
        return;
    }

    size_t index = 0;
    const char* cursor = format;

    while ((*cursor != '\0') && (index < (bufferSize - 1))) {
        if (*cursor != '%') {
            appendChar(buffer, bufferSize, index, *cursor);
            ++cursor;
            continue;
        }

        if (*(cursor + 1) == '%') {
            appendChar(buffer, bufferSize, index, '%');
            cursor += 2;
            continue;
        }

        FormatSpecifier spec;
        if (!parseFormatSpecifier(cursor, spec)) {
            appendChar(buffer, bufferSize, index, *cursor);
            ++cursor;
            continue;
        }

        char chunk[LOGGER_CHUNK_SIZE];
        int written = formatSpecifierValue(chunk, sizeof(chunk), spec, args, index);
        if (written < 0) {
            appendData(buffer, bufferSize, index, spec.begin, static_cast<size_t>(spec.end - spec.begin));
        } else if (written > 0) {
            size_t chunkLength = static_cast<size_t>(written);
            if (chunkLength >= sizeof(chunk)) {
                chunkLength = sizeof(chunk) - 1;
            }
            appendData(buffer, bufferSize, index, chunk, chunkLength);
        }

        cursor = spec.end;
    }
}

static void writeLogMessage(const char* level, const char* format, va_list& args, bool appendNewline) {
    if (level != NULL) {
        Serial.print("[");
        Serial.print(level);
        Serial.print("] ");
    }

    char buffer[LOGGER_BUFFER_SIZE];
    formatLogMessage(buffer, sizeof(buffer), format, args);

    if (appendNewline) {
        Serial.println(buffer);
    } else {
        Serial.print(buffer);
    }
}

}  // namespace

#ifndef USE_AUDIO_LOGGING
int currentLogLevel = LOG_LEVEL_DEBUG; // Default log level
#endif

void uint8ToBinaryString(char *buffer, uint8_t value) {
    for (int i = 7; i >= 0; i--) {
        buffer[7 - i] = ((value >> i) & 1) ? '1' : '0';
    }
    buffer[8] = '\0'; // Null-terminate the string
}

void logPrintLevelln(const char* level, const char* format, ...) {
    va_list args;
    va_start(args, format);
    writeLogMessage(level, format, args, true);
    va_end(args);
}

void logPrintLevel(const char* level, const char* format, ...) {
    va_list args;
    va_start(args, format);
    writeLogMessage(level, format, args, false);
    va_end(args);
}

void logPrint(const char* format, ...) {
    va_list args;
    va_start(args, format);
    writeLogMessage(NULL, format, args, false);
    va_end(args);
}

void logPrintln(const char* format, ...) {
    va_list args;
    va_start(args, format);
    writeLogMessage(NULL, format, args, true);
    va_end(args);
}
