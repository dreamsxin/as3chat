/* 
 * File:   Log.h
 * Author: phpwind
 *
 * Created on 2010年12月17日, 下午1:29
 */

#ifndef LOG_H
#define	LOG_H
#include <stdio.h>
#include <stdarg.h>

typedef enum {
    LOG_PARANOID,
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
} log_level_t;

class Log {
public:
    Log();
    virtual ~Log();
    
    static void setLogLevel(log_level_t lvl);
    static log_level_t getLogLevel();

    static void debug(const char *fmt, ...);
    static void error(const char *fmt, ...);
    static void print(log_level_t lvl, const char *fmt, ...);
private:
    static log_level_t level;
};

#endif	/* LOG_H */

