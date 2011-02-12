/* 
 * File:   Log.cpp
 * Author: phpwind
 * 
 * Created on 2010年12月17日, 下午1:29
 */

#include "Log.h"

log_level_t Log::level = LOG_DEBUG;

Log::Log() {
}

Log::~Log() {
}

void Log::setLogLevel(log_level_t lvl) {
    level = lvl;
}

log_level_t Log::getLogLevel() {
    return level;
}

void Log::debug(const char *fmt, ...) {
    if (fmt != NULL && LOG_DEBUG >= level) {
        const char *offset = fmt;
        va_list args;
        va_start(args, fmt);
        vfprintf(stderr, offset, args);
        va_end(args);
    }
}

void Log::error(const char *fmt, ...) {
    if (fmt != NULL && LOG_ERROR >= level) {
        const char *offset = fmt;
        va_list args;
        va_start(args, fmt);
        vfprintf(stderr, offset, args);
        va_end(args);
    }
}

void Log::print(log_level_t lvl, const char *fmt, ...) {
    if (fmt != NULL && lvl >= level) {
        const char *offset = fmt;
        va_list args;
        va_start(args, fmt);
        vfprintf(stderr, offset, args);
        va_end(args);
    }
}

