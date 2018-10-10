/*! \file    debug.h
 * \author   liangganggang <944922198@qq.com>
 * \copyright GNU General Public License v3
 * \brief    Logging and Debugging
 * \details  Implementation of a wrapper on printf (or g_print) to either log or debug.
 * \todo     Improve this wrappers to optionally save logs on file
 *
 * \ingroup core
 * \ref core
 */

#ifndef _ANNA_DEBUG_H
#define _ANNA_DEBUG_H
#include <time.h>
#include "log.h"

extern int anna_log_level;
extern bool anna_log_timestamps;
extern bool anna_log_colors;

/** @name Janus log colors
 */
///@{
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
///@}

/** @name Janus log levels
 */
///@{
/*! \brief No debugging */
#define LOG_NONE     (0)
/*! \brief Fatal error */
#define LOG_FATAL    (1)
/*! \brief Non-fatal error */
#define LOG_ERR      (2)
/*! \brief Warning */
#define LOG_WARN     (3)
/*! \brief Informational message */
#define LOG_INFO     (4)
/*! \brief Verbose message */
#define LOG_VERB     (5)
/*! \brief Overly verbose message */
#define LOG_HUGE     (6)
/*! \brief Debug message (includes .c filename, function and line number) */
#define LOG_DBG      (7)
/*! \brief Maximum level of debugging */
#define LOG_MAX LOG_DBG

/*! \brief Coloured prefixes for errors and warnings logging. */
static const char *anna_log_prefix[] = {
/* no colors */
        "",
        "[FATAL] ",
        "[ERR] ",
        "[WARN] ",
        "",
        "",
        "",
        "[DBG] ",
/* with colors */
        "",
        ANSI_COLOR_MAGENTA"[FATAL]"ANSI_COLOR_RESET" ",
        ANSI_COLOR_RED"[ERR]"ANSI_COLOR_RESET" ",
        ANSI_COLOR_YELLOW"[WARN]"ANSI_COLOR_RESET" ",
        "",
        "",
        "",
        ANSI_COLOR_CYAN"[BDG]"ANSI_COLOR_RESET" "
};


///@}

/** @name Janus log wrappers
 */
///@{
/*! \brief Simple wrapper to g_print/printf */
#define ANNA_PRINT anna_vprintf
/*! \brief Logger based on different levels, which can either be displayed
 * or not according to the configuration of the gateway.
 * The format must be a string literal. */
#define ANNA_LOG(level, format, ...) \
do { \
    if (level > LOG_NONE && level <= LOG_MAX && level <= anna_log_level) { \
        char anna_log_ts[64] = ""; \
        char anna_log_src[128] = ""; \
        if (anna_log_timestamps) { \
                    struct tm annatmresult; \
                    time_t annaltime = time(NULL); \
                    localtime_r(&annaltime, &annatmresult); \
                    strftime(anna_log_ts, sizeof(anna_log_ts), \
                         "[%a %b %e %T %Y] ", &annatmresult); \
        } \
        if (level == LOG_FATAL || level == LOG_ERR || level == LOG_DBG) { \
            snprintf(anna_log_src, sizeof(anna_log_src), \
            "[%s:%s:%d] ", __FILE__, __FUNCTION__, __LINE__); \
        } \
        ANNA_PRINT("%s%s%s" format, \
        anna_log_ts, \
        anna_log_prefix[level | ((int)anna_log_colors << 3)], \
        anna_log_src, \
        ##__VA_ARGS__); \
    } \
} while (0)
///@}

#endif

