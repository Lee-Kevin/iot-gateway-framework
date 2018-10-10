/*! \file    log.h
 * \author   liangganggang <944922198@qq.com>
 * \copyright GNU General Public License v3
 * \brief    Buffered logging (headers)
 * \details  Implementation of a simple buffered logger designed to remove
 * I/O wait from threads that may be sensitive to such delays. Buffers are
 * saved and reused to reduce allocation calls. The logger output can then
 * be printed to stdout and/or a log file.
 *
 * \ingroup core
 * \ref core
 */

#ifndef _ANAN_LOG_H
#define _ANAN_LOG_H

#include <stdio.h>
#include <stdbool.h>

/*! \brief Buffered vprintf
* @param[in] format Format string as defined by glib
* \note This output is buffered and may not appear immediately on stdout. */
void anna_vprintf(const char *format, ...);

/*! \brief Log initialization
* \note This should be called before attempting to use the logger. A buffer
* pool and processing thread are created.
* @param daemon Whether the Janus is running as a daemon or not
* @param console Whether the output should be printed on stdout or not
* @param logfile Log file to save the output to, if any
* @returns 0 in case of success, a negative integer otherwise */
int anna_log_init(bool daemon, bool console, const char *logfile);
/*! \brief Log destruction */
void anna_log_destroy(void);

/*! \brief Method to check whether stdout logging is enabled
 * @returns TRUE if stdout logging is enabled, FALSE otherwise */
bool anna_log_is_stdout_enabled(void);
/*! \brief Method to check whether file-based logging is enabled
 * @returns TRUE if file-based logging is enabled, FALSE otherwise */
bool anna_log_is_logfile_enabled(void);
/*! \brief Method to get the path to the log file
 * @returns The full path to the log file, or NULL otherwise */
char *anna_log_get_logfile_path(void);

#endif

