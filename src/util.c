#include "util.h"
#include <stdarg.h>

void print(const char* msg, ...) {
    #ifdef VERBOSE
        va_list args;
        va_start(args, msg);
        vprintf(msg, args);
        va_end(args);
    #endif
}

void print_error(const char* msg, ...) {
    va_list args;
    va_start(args, msg);
    vfprintf(stderr, msg, args);
    va_end(args);
}
